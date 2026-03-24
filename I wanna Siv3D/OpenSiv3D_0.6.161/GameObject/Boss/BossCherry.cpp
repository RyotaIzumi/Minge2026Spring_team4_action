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
		TextureAsset(U"sprCherryLow").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1,ColorF(1.0, isMuteki ? 0.6 : 1.0));
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
	}

	// 攻撃を呼び出す
	void BossSubCherry::generateAttack(BossCherryType type) {
		if (cherrySubType == type) {
			switch (cherrySubType) {
			case BossCherryType::Red:
				bossStageManager->createSubThrowCherry(120, 9, [this]() { return std::make_shared<BossSubThrowCherry>(pos,2.0,cherrySubType,*bossStageManager); });
				break;
			case BossCherryType::Blue:

				break;
			}
		}
	}

	void BossSubCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLow").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1, ColorF(1.0, isMuteki ? 0.6 : 1.0));
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}

	void BossSubCherry::setCenterPos(Vec2 cPos) {
		centerPos = cPos;
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
	}

	void BossBarrageCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLow").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1, ColorF(1.0, isMuteki ? 0.6 : 1.0));
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
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

		gravity = 0.3;

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
		TextureAsset(U"sprCherryLow").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1, ColorF(1.0, isMuteki ? 0.6 : 1.0));
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}
}
