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

		hasHp = true;
		maxHp = 25;
		hp = maxHp;

		baseCenterPos = Vec2{ 400, 350 };
		speed = 20;
		direction = 90;
		gravity = 0;

		depth = 51;

		c = 0;
		r = 20;
		attackIntervalTime = 3.5;
		startStep = 0;

		specialAttackStep = 0;
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
				isSpecialAttack = true;
				startStep++;
			}
			break;
		case 2://浮遊処理
			pos.y = -r * sin(Math::ToRadians(c)) + baseCenterPos.y;
			c += 1;
			break;
		}

		//開始時の攻撃
		if (startAttackTimer.reachedZero()) {
			cherryAttackType = canAttackTypes.choice();
			startAttack(cherryAttackType);
			startAttackTimer.reset();
		}

		//hp表示のフェードイン
		if (hpBarAlpha < 1)hpBarAlpha += 0.05;

		//攻撃間隔の設定
		switch (defeatedAttackTypeNum) {
		case 0:attackIntervalTime = 3.5; break;
		case 1:attackIntervalTime = 3.2; break;
		case 2:attackIntervalTime = 2.8; break;
		case 3:attackIntervalTime = 2.4; break;
		case 4:attackIntervalTime = 2.0; break;
		case 5:attackIntervalTime = 1.5; break;
		}

		//攻撃強化関連の処理
		if (hp < maxHp / 2 || defeatedAttackTypeNum >= 3) {
			Global::isBossAttackPowerUp = true;
			if (attackIntervalTime > 2.0)attackIntervalTime = 2.0;
		}

		/*
		//特殊攻撃の呼び出し
		if (isSpecialAttack) {
			switch (specialAttackStep) {
			case 0:
				bossStageManager->createGrayLatticeCherry(100,[this]() { return std::make_shared<BossGrayLatticeCherry>(pos, 1.0, BossCherryType::Gray); });
				AudioAsset(Sound::BLOCKCHANGE).playOneShot();
				specialAttackStep++;
				break;
			case 1:

				break;
			}
			return;
		}
		*/

		// 一定間隔でファンネルりんごを一つ選んで攻撃
		if (reachedAttackTime(attackIntervalTime)) {
			if(!canAttackTypes.empty())cherryAttackType = canAttackTypes.choice();
			attackStopwatch.restart();
		}
		else {
			cherryAttackType = BossCherryType::None;
		}

		if (hp <= 0) {
			throw Error(U"おめでとう! キミはボスを撃破した！！");
		}
	}

	void BossCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLowBoss").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1,ColorF(1.0, isMuteki ? 0.6 : 1.0));

		// ===== HPバー =====
		if (hasHp) {
			double width = Global::stageWidth;   // 横幅
			double height = 20;              // 高さ
			Vec2 barPos = Vec2(400,0);

			// 最大HP（赤）
			RectF(barPos.x - width / 2, barPos.y, width, height)
				.draw(ColorF(1.0, 0.2, 0.2, hpBarAlpha));

			// 現在HP（緑）
			double hpRate = static_cast<double>(hp) / maxHp;
			RectF(barPos.x - width / 2, barPos.y, width * hpRate, height)
				.draw(ColorF(0.2, 1.0, 0.2, hpBarAlpha));

			// 文字表示
			Vec2 textBasePos = Vec2(6, 18);
			FontAsset(U"BossHp")(U"Boss : Guardian Cherry").draw(textBasePos.x - 1, textBasePos.y, ColorF(0, 0, 0, hpBarAlpha));
			FontAsset(U"BossHp")(U"Boss : Guardian Cherry").draw(textBasePos.x + 1, textBasePos.y, ColorF(0, 0, 0, hpBarAlpha));
			FontAsset(U"BossHp")(U"Boss : Guardian Cherry").draw(textBasePos.x, textBasePos.y - 1, ColorF(0, 0, 0, hpBarAlpha));
			FontAsset(U"BossHp")(U"Boss : Guardian Cherry").draw(textBasePos.x, textBasePos.y + 1, ColorF(0, 0, 0, hpBarAlpha));

			// 本体（白）
			FontAsset(U"BossHp")(U"Boss : Guardian Cherry").draw(textBasePos.x, textBasePos.y, ColorF(1.0, 1.0, 1.0, hpBarAlpha));
		}
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}

	// 攻撃を呼び出す
	void BossCherry::startAttack(BossCherryType type) {
		double throwDir, throwSpd;
		int32 attackCount = 2;
		for (int32 i = 0; i < attackCount; i++) {
			switch (type) {
			case BossCherryType::Red:
				throwDir = 60 + Random(60);
				throwSpd = 4 + Random(7);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this, type]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, type, *bossStageManager); });
				break;
			case BossCherryType::Blue:
				throwDir = 70 + Random(40);
				throwSpd = 14 + Random(3);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this, type]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, type, *bossStageManager); });
				return;
				break;
			case BossCherryType::Yellow:
				throwDir = 60 + Random(60);
				throwSpd = 5 + Random(6);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this, type]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, type, *bossStageManager); });
				break;
			case BossCherryType::Green:
				throwDir = 70 + Random(40);
				throwSpd = 4 + Random(7);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this, type]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, type, *bossStageManager); });
				break;
			case BossCherryType::Orange:
				throwDir = 70 + Random(40);
				throwSpd = 4 + Random(7);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this, type]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, type, *bossStageManager); });
				break;
			case BossCherryType::Sky:
				throwDir = 70 + Random(40);
				throwSpd = 4 + Random(7);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this, type]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, type, *bossStageManager); });
				break;
			}
		}
	}

	//次に攻撃するりんごの種類を取得
	BossCherryType BossCherry::getBossCherryAttackType() const {
		return cherryAttackType;
	}

	//攻撃できるりんごの種類を設定
	void BossCherry::removeDefeatedAttackType(BossCherryType type) {
		defeatedAttackTypeNum++;
		canAttackTypes.remove(type);
	}

	// 倒した攻撃の種類の数を取得
	int32 BossCherry::getDefeatedBossNum() const {
		return defeatedAttackTypeNum;
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

		hasHp = true;
		maxHp = 7;
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

		switch (defeatedBossNum) {
		case 1:c += 0.2;break;
		case 2:c += 0.5;break;
		case 3:c += 1.0;break;
		case 4:c += 3.0;break;
		case 5:c += 5.0;break;
		case 6:c += 10.0;break;
		}

		pos.x = r * cos(Math::ToRadians(c)) + centerPos.x;
		pos.y = -r * sin(Math::ToRadians(c)) + centerPos.y;

		setTypeColor();
	}

	void BossSubCherry::draw() const {

		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLowBarrageWhite").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1, typeColor);
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化

		// ===== HPバー =====
		if (hasHp && isMuteki) {
			double width = 32 * scaleMag;   // 横幅
			double height = 4;              // 高さ
			Vec2 barPos = pos + Vec2(0, -10 * scaleMag); // 上に表示

			// 最大HP（赤）
			RectF(barPos.x - width / 2, barPos.y, width, height)
				.draw(ColorF(1.0, 0.2, 0.2));

			// 現在HP（緑）
			double hpRate = static_cast<double>(hp) / maxHp;
			RectF(barPos.x - width / 2, barPos.y, width * hpRate, height)
				.draw(ColorF(0.2, 1.0, 0.2));
		}
	}

	void BossSubCherry::setCenterPos(Vec2 cPos) {
		centerPos = cPos;
	}

	BossCherryType BossSubCherry::getBossCherrySubType() const {
		return cherrySubType;
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

	// 倒したボスの数を設定
	void BossSubCherry::setDefeatedBossNum(int32 num) {
		defeatedBossNum = num;
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
				throwDir = 70 + Random(40);
				throwSpd = 14 + Random(3);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, cherrySubType, *bossStageManager); });
				break;
			case BossCherryType::Yellow:
				throwDir = 60 + Random(60);
				throwSpd = 9 + Random(3);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, cherrySubType, *bossStageManager); });
				break;
			case BossCherryType::Green:
				throwDir = 75 + Random(30);
				throwSpd = 9 + Random(3);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, cherrySubType, *bossStageManager); });
				break;
			case BossCherryType::Orange:
				throwDir = 70 + Random(40);
				throwSpd = 9 + Random(3);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, cherrySubType, *bossStageManager); });
				break;
			case BossCherryType::Sky:
				throwDir = 60 + Random(60);
				throwSpd = 11 + Random(3);
				bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, cherrySubType, *bossStageManager); });
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

		alpha = 1.0;
		startStep = 0;
	}

	void BossBarrageCherry::barrageUpdate() {
		setTypeColor();
	}

	void BossBarrageCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLowBarrageWhite").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1, typeColor);
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
		case BossCherryType::Gray:   typeColor = ColorF(Palette::Gray, alpha); break;
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

			switch (cherrySubType) {
				case BossCherryType::Red:
				case BossCherryType::Blue:
				case BossCherryType::Yellow:
				case BossCherryType::Sky:
					if (splitTimer.reachedZero()) {
						split();
						startStep++;
					}
					break;

				case BossCherryType::Green:
				case BossCherryType::Orange:
					if (pos.y > Global::stageHeight) {
						split();
						startStep++;
					}
					break;
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
		//攻撃強化状態前
		if (!Global::isBossAttackPowerUp) {
			switch (cherrySubType) {
			case BossCherryType::Red:
				bossStageManager->createCherrySpread(25, 6, [this]() { return std::make_shared<BossBarrageCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::BLOCKBREAK).playOneShot();
				break;
			case BossCherryType::Blue:
				bossStageManager->createBlueLineCherry(20, 80, [this]() { return std::make_shared<BossFallBlueCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::CHERRYFALL).playOneShot();
				break;
			case BossCherryType::Yellow:
				bossStageManager->createYellowStarCherry(5, 1, pos, 6, [this]() { return std::make_shared<BossYellowStarCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::BLOCKBREAK).playOneShot();
				break;
			case BossCherryType::Green:
				bossStageManager->createGreenWaveCherry(pos, 0.05,10, [this]() { return std::make_shared<BossGreenWaveCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::BLOCKBREAK).playOneShot();
				break;
			case BossCherryType::Orange:
				bossStageManager->createOrangeStopCherry(false, [this]() { return std::make_shared<BossOrangeStopCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::SPIKETRAP).playOneShot();
				break;
			case BossCherryType::Sky:
				bossStageManager->createSkyTargetCherry(7, false, [this]() { return std::make_shared<BossSkyTargetCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::BLOCKBREAK).playOneShot();
				break;
			}
		}
		else {
			switch (cherrySubType) {
			case BossCherryType::Red:
				bossStageManager->createCherrySpread(20, 6, [this]() { return std::make_shared<BossBarrageCherry>(pos, 1.0, cherrySubType); });
				bossStageManager->createCherrySpread(25, 4, [this]() { return std::make_shared<BossBarrageCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::BLOCKBREAK).playOneShot();
				break;
			case BossCherryType::Blue:
				bossStageManager->createBlueLineCherry(40, 50, [this]() { return std::make_shared<BossFallBlueCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::CHERRYFALL).playOneShot();
				break;
			case BossCherryType::Yellow:
				bossStageManager->createYellowStarCherry(5, 2, pos, 10, [this]() { return std::make_shared<BossYellowStarCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::BLOCKBREAK).playOneShot();
				break;
			case BossCherryType::Green:
				bossStageManager->createGreenWaveCherry(pos, 0.05, 13, [this]() { return std::make_shared<BossGreenWaveCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::BLOCKBREAK).playOneShot();
				break;
			case BossCherryType::Orange:
				bossStageManager->createOrangeStopCherry(true, [this]() { return std::make_shared<BossOrangeStopCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::SPIKETRAP).playOneShot();
				break;
			case BossCherryType::Sky:
				bossStageManager->createSkyTargetCherry(5, true, [this]() { return std::make_shared<BossSkyTargetCherry>(pos, 1.0, cherrySubType); });
				AudioAsset(Sound::BLOCKBREAK).playOneShot();
				break;
			}
		}
	}

	void BossSubThrowCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLowBarrageWhite").scaled(scaleMag).rotated(rotateC).drawAt(pos.x - 1, pos.y - 1, typeColor);
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}
}
