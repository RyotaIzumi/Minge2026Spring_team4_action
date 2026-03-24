#include "BossCherry.h"
#include "../../Audio/AudioAsset.h"
#include "../../StageManager/BossStageManager.h"

namespace Iwanna {
	BossCherry::BossCherry(Vec2 startPos, double scale, BossStageManager& manager) : bossStageManager(& manager), Cherry(startPos, scale) {

		//GameObject.hの値初期化
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);
		type = ObjectType::Cherry;
		cherryType = CherryType::Boss;
		cherryAttackType = BossCherryType::None;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = false;
		isTrap = false;

		hasHp = true;
		maxHp = 30;
		hp = maxHp;

		baseCenterPos = Vec2{ 400, 350 };
		speed = 20;
		direction = 90;
		gravity = 0;

		c = 0;
		r = 20;
		attackIntervalTime = 3.5;
		startStep = 0;
	}

	void BossCherry::barrageUpdate() {
		//ボス戦開始時の処理
		switch (startStep) {
		case 0:
			if (pos.y < -300) {
				pos.x = 400;
				pos.y = -300;
				speed = 0;
				movePosition(baseCenterPos, 1.0, false);
				startStep++;
			}
			break;
		case 1:
			if (getIsMoveFinished()) {
				attackStopwatch.restart();//攻撃の開始
				attackIntervalTime = 3.5;
				startStep++;
			}
			break;
		case 2://浮遊処理
			pos.y = -r * sin(Math::ToRadians(c)) + baseCenterPos.y;
			c += 1;
			break;
		}

		// 一定間隔でファンネルりんごを一つ選んで攻撃
		if (reachedAttackTime(attackIntervalTime)) {
			cherryAttackType = BossCherryType::Red;
			attackStopwatch.restart();
		}
		else {
			cherryAttackType = BossCherryType::None;
		}
	}

	void BossCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLowBoss").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1,ColorF(1.0, isMuteki ? 0.6 : 1.0));
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}

	//次に攻撃するりんごの種類を取得
	BossCherryType BossCherry::getBossCherryAttackType() const {
		return cherryAttackType;
	}

	// 攻撃間隔が指定時間に達したかどうかを取得
	bool BossCherry::reachedAttackTime(double time) {
		return attackStopwatch.s() >= time;
	}

	// ----- ボスがまとうファンネルりんご ----- //
	BossSubCherry::BossSubCherry(Vec2 startPos, double scale, BossCherryType cType, BossStageManager& manager) : bossStageManager(&manager), Cherry(startPos, scale) {

		//GameObject.hの値初期化
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);
		type = ObjectType::Cherry;
		cherryType = CherryType::BossSub;
		cherrySubType = cType;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = false;
		isTrap = false;

		hasHp = true;
		maxHp = 10;
		hp = maxHp;

		speed = 0;
		direction = 0;
		gravity = 0;

		startStep = 0;

		r = 0;
		rMax = 85;
		c = static_cast<double>(cType) * 60;
	}

	void BossSubCherry::barrageUpdate() {
		//ボス戦開始時の処理
		switch (startStep) {
		case 0:
			if (startTimer.reachedZero()) {
				movePosition(pos, 1.0, false);
				rTimer.restart();
				startStep++;
			}
			break;
		case 1:
			r = rMax * EaseOutQuad(rTimer.progress0_1());
			if (rTimer.reachedZero()) {
				startStep++;
			}
			break;
		case 2:
			break;
		}

		pos.x = r * cos(Math::ToRadians(c)) + centerPos.x;
		pos.y = -r * sin(Math::ToRadians(c)) + centerPos.y;

		setTypeColor();
	}

	void BossSubCherry::draw() const {

		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLowWhite").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1, typeColor);
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}

	void BossSubCherry::setCenterPos(Vec2 cPos) {
		centerPos = cPos;
	}

	// 種類で色を決定する
	void BossSubCherry::setTypeColor() {
		alpha = isMuteki ? 0.6 : 1.0;
		switch (cherrySubType) {
		case BossCherryType::Red:    typeColor = ColorF(Palette::Red, alpha); break;
		case BossCherryType::Blue:   typeColor = ColorF(Palette::Blue,alpha); break;
		case BossCherryType::Yellow: typeColor = ColorF(Palette::Yellow,alpha); break;
		case BossCherryType::Green:  typeColor = ColorF(Palette::Greenyellow,alpha); break;
		case BossCherryType::Orange: typeColor = ColorF(Palette::Orange,alpha); break;
		case BossCherryType::Sky:    typeColor = ColorF(Palette::Skyblue,alpha); break;
		}
	}

	// 攻撃を呼び出す
	void BossSubCherry::generateAttack(BossCherryType type) {
		double throwDir, throwSpd;
		if (cherrySubType == type) {
			switch (cherrySubType) {
			case BossCherryType::Red:
				throwDir = 60 + Random(60);
				throwSpd = 9 + Random(3);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos,2.0,cherrySubType,*bossStageManager); });
				break;
			case BossCherryType::Blue:

				break;
			}
		}
	}

	// ----- 弾幕用りんご ----- //
	BossBarrageCherry::BossBarrageCherry(Vec2 startPos, double scale, BossCherryType cType) : Cherry(startPos, scale) {

		//GameObject.hの値初期化
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);
		type = ObjectType::Cherry;
		cherryType = CherryType::Barrage;
		cherrySubType = cType;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = true;
		isTrap = false;

		alpha = 1.0;
		startStep = 0;
	}

	void BossBarrageCherry::barrageUpdate() {
		//ボス戦開始時の処理
		switch (startStep) {
		case 0:

			break;
		case 1:

			break;
		case 2:
			break;
		}

		setTypeColor();
	}

	void BossBarrageCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLowWhite").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1, typeColor);
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}

	// 種類で色を決定する
	void BossBarrageCherry::setTypeColor() {
		switch (cherrySubType) {
		case BossCherryType::Red:    typeColor = ColorF(Palette::Red, alpha); break;
		case BossCherryType::Blue:   typeColor = ColorF(Palette::Blue, alpha); break;
		case BossCherryType::Yellow: typeColor = ColorF(Palette::Yellow, alpha); break;
		case BossCherryType::Green:  typeColor = ColorF(Palette::Lawngreen, alpha); break;
		case BossCherryType::Orange: typeColor = ColorF(Palette::Orange, alpha); break;
		case BossCherryType::Sky:    typeColor = ColorF(Palette::Skyblue, alpha); break;
		}
	}

	// ----- ファンネルりんごが投げるでかりんご ----- //
	BossSubThrowCherry::BossSubThrowCherry(Vec2 startPos, double scale, BossCherryType cType, BossStageManager& manager) : bossStageManager(&manager),BossBarrageCherry(startPos, scale, cType) {

		//GameObject.hの値初期化
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);
		type = ObjectType::Cherry;
		cherryType = CherryType::Barrage;
		cherrySubType = cType;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = false;
		isTrap = false;

		gravity = 0.25;

		startStep = 0;
	}

	void BossSubThrowCherry::barrageUpdate() {
		switch (startStep) {
		case 0:
			splitTimer.restart();
			startStep++;
			break;
		case 1://破裂して弾幕生成
			if (splitTimer.reachedZero()) {
				split();
				startStep++;
			}
			break;
		case 2:
			isDelete = true;
			break;
		}

		//回転方向を決定する
		const double rightAngle = 90;//直角
		isRorateLeft = direction > rightAngle;
		if (!isRorateLeft)rotateC += addRorateC;
		else rotateC -= addRorateC;

		setTypeColor();
	}

	// 破裂時にりんごを生成する
	void BossSubThrowCherry::split() {
		switch (cherrySubType) {
		case BossCherryType::Red:
			bossStageManager->createCherrySpread(30,6, [this]() { return std::make_shared<BossBarrageCherry>(pos, 1.0, cherrySubType); });
			break;
		}
	}

	void BossSubThrowCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLowWhite").scaled(scaleMag).rotated(rotateC).drawAt(pos.x - 1, pos.y - 1, typeColor);
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}
}
