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
		hpBarDelay.reset(hp, maxHp);

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
				startStep++;
			}
			break;
		case 2://浮遊処理
			pos.y = -r * sin(Math::ToRadians(c)) + baseCenterPos.y;
			c += 1;
			break;
		case 3:
			gravity = 0.3;
			speed = 1;
			direction = 270;
			startStep++;
			break;
		}

		if (Global::isBossDefeated) return;

		//開始時の攻撃
		if (startAttackTimer.reachedZero()) {
			cherryAttackType = canAttackTypes.choice();
			startAttack(cherryAttackType);
			startAttackTimer.reset();
		}

		//hp表示のフェードイン
		if (hpBarAlpha < 1)hpBarAlpha += 0.05;
		hpBarDelay.update(hp, maxHp);

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
			if(specialAttackCount == 0)isSpecialAttack = true;
		}

		if(defeatedAttackTypeNum >= 6) isSpecialAttack = true;
		
		//特殊攻撃の呼び出し
		if (isSpecialAttack) {
			switch (specialAttackStep) {
			case 0:
				bossStageManager->createGrayLatticeCherry(100,[this]() { return std::make_shared<BossGrayLatticeCherry>(pos, 1.0, BossCherryType::Gray); });
				Sound::playOneShot(Sound::BLOCKCHANGE);
				specialAttackStopwatch.restart();
				specialAttackCount++;
				specialAttackStep++;
				break;
			case 1:
				if (specialAttackStopwatch.sF() > specialAttackIntervalTime) {
					specialAttackStep = 0;
					isSpecialAttack = false;
				}
				break;
			}
			return;
		}

		// 一定間隔でファンネルりんごを一つ選んで攻撃
		if (reachedAttackTime(attackIntervalTime)) {
			if(!canAttackTypes.empty())cherryAttackType = canAttackTypes.choice();
			attackStopwatch.restart();
		}
		else {
			cherryAttackType = BossCherryType::None;
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

			drawBossHpBar(barPos, width, height, hp, maxHp, hpBarAlpha, hpBarDelay);

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

	//ダメージを受けた際の処理
	void BossCherry::hited() {
		if (hp > 0) {
			Sound::playOneShot(Sound::BOSSHIT);
			hp--;
		}

		if (hp <= 0) {
			Sound::playOneShot(Sound::DEATH);
			alpha = 0;
			startStep = 3;
			Global::isBossDefeated = true;
		}

		isMuteki = true;
		mutekiInterval.restart();
	}

	TayamaBoss::TayamaBoss(Vec2 startPos, double scale, BossStageManager& manager)
		: bossStageManager(&manager), Cherry(startPos, scale) {
		pos = startPos;
		baseScaleMag = scale;
		setScaleMag(scale);
		type = ObjectType::Cherry;
		cherryType = CherryType::TrapBoss;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = false;

		hasHp = true;
		maxHp = 20;
		hp = maxHp;
		hpBarDelay.reset(hp, maxHp);

		gravity = 0;
		speed = 0;
		depth = 51;

		const Vec2 centerPos{ Global::stageWidth / 2.0, Global::stageHeight / 2.0 };
		movePosition(centerPos, 1.5, false);

		switch (Random(2)) {
		case 0:
			Sound::playOneShot(Sound::VC_HOSO1);
			break;
		case 1:
			Sound::playOneShot(Sound::VC_HOSO2);
			break;
		case 2:
			Sound::playOneShot(Sound::VC_HOSO3);
			break;
		}
	}

	void TayamaBoss::applyScaleMag(double scale) {
		scaleMag = Max(0.0, scale);
		hitBox = std::make_shared<CircleHitBox>(pos, 45.0 * scaleMag);
	}

	void TayamaBoss::setScaleMag(double scale) {
		baseScaleMag = Max(0.0, scale);
		applyScaleMag(baseScaleMag);
	}

	void TayamaBoss::setRotatingSpreadAttackSettings(double duration, double interval) {
		rotatingSpreadAttackDuration = Max(0.1, duration);
		rotatingSpreadAttackInterval = Max(0.05, interval);
	}

	void TayamaBoss::setTargetAttackSettings(double duration, double interval) {
		targetAttackDuration = Max(0.1, duration);
		targetAttackInterval = Max(0.05, interval);
	}

	void TayamaBoss::setTargetAttackSpeedSettings(double baseSpeed, double intervalSpeed) {
		targetAttackBaseSpeed = Max(0.0, baseSpeed);
		targetAttackIntervalSpeed = intervalSpeed;
	}

	void TayamaBoss::setLineAttackSettings(double moveDuration, double targetY, double cherryInterval) {
		lineAttackMoveDuration = Max(0.1, moveDuration);
		lineAttackTargetY = targetY;
		lineAttackCherryInterval = Max(4.0, cherryInterval);
	}

	void TayamaBoss::setLineAttackWarningSettings(double warnScale, double scaleUpTime, double scaleDownTime, double generateWaitTime) {
		lineAttackWarnScale = Max(1.0, warnScale);
		lineAttackScaleUpTime = Max(0.05, scaleUpTime);
		lineAttackScaleDownTime = Max(0.05, scaleDownTime);
		lineAttackGenerateWaitTime = Max(0.0, generateWaitTime);
	}

	void TayamaBoss::startRandomAttack() {
		if (attackCountInSet == 0) {
			targetAttackCountInSet = Random(1, 2);
		}

		if (attackCountInSet < 3) {
			currentAttackPattern = (attackCountInSet == targetAttackCountInSet) ? 1 : 0;
		}
		else {
			currentAttackPattern = 2;
		}

		switch (currentAttackPattern) {
		case 0:
			startRotatingSpreadAttack();
			break;
		case 1:
			startTargetAttack();
			break;
		case 2:
			startLineAttack();
			break;
		default:
			startRotatingSpreadAttack();
			break;
		}
	}

	void TayamaBoss::startRotatingSpreadAttack() {
		currentAttackPattern = 0;
		attackStopwatch.restart();
		attackIntervalStopwatch.restart();
		bossStageManager->createCherrySpread(rotatingSpreadCherryNum, rotatingSpreadCherrySpeed, [this]() { return createTrapBarrageCherry(); });
	}

	void TayamaBoss::updateRotatingSpreadAttack() {
		textureAngle += rotatingSpreadRotateSpeed * Scene::DeltaTime();

		if (attackIntervalStopwatch.sF() >= rotatingSpreadAttackInterval) {
			bossStageManager->createCherrySpread(rotatingSpreadCherryNum, rotatingSpreadCherrySpeed, [this]() { return createTrapBarrageCherry(); });
			attackIntervalStopwatch.restart();
		}

		if (attackStopwatch.sF() >= rotatingSpreadAttackDuration) {
			finishAttack();
		}
	}

	void TayamaBoss::startTargetAttack() {
		currentAttackPattern = 1;
		attackStopwatch.restart();
		attackIntervalStopwatch.restart();
		bossStageManager->createSkyTargetCherry(targetAttackLineNum, targetAttackIsAddLine, [this]() { return createTrapSkyTargetCherry(); }, targetAttackBaseSpeed, targetAttackIntervalSpeed);
	}

	void TayamaBoss::updateTargetAttack() {
		if (attackIntervalStopwatch.sF() >= targetAttackInterval) {
			bossStageManager->createSkyTargetCherry(targetAttackLineNum, targetAttackIsAddLine, [this]() { return createTrapSkyTargetCherry(); }, targetAttackBaseSpeed, targetAttackIntervalSpeed);
			attackIntervalStopwatch.restart();
		}

		if (attackStopwatch.sF() >= targetAttackDuration) {
			finishAttack();
		}
	}

	void TayamaBoss::startLineAttack() {
		currentAttackPattern = 2;
		lineAttackStep = 0;
		attackStopwatch.restart();
		attackIntervalStopwatch.restart();
		Sound::playOneShot(Sound::VC_PON);
	}

	void TayamaBoss::updateLineAttack() {
		const double warnedScale = baseScaleMag * lineAttackWarnScale;

		switch (lineAttackStep) {
		case 0:
			applyScaleMag(Math::Lerp(baseScaleMag, warnedScale, Min(1.0, attackStopwatch.sF() / lineAttackScaleUpTime)));
			if (attackStopwatch.sF() >= lineAttackScaleUpTime) {
				attackStopwatch.restart();
				lineAttackStep = 1;
			}
			break;
		case 1:
			applyScaleMag(Math::Lerp(warnedScale, baseScaleMag, Min(1.0, attackStopwatch.sF() / lineAttackScaleDownTime)));
			if (attackStopwatch.sF() >= lineAttackScaleDownTime) {
				applyScaleMag(baseScaleMag);
				attackStopwatch.restart();
				lineAttackStep = 2;
			}
			break;
		case 2:
			if (attackStopwatch.sF() >= lineAttackGenerateWaitTime) {
				generateLineAttack();
				attackStopwatch.restart();
				Sound::playOneShot(Sound::SPIKETRAP);
				lineAttackStep = 3;
			}
			break;
		case 3:
			if (attackStopwatch.sF() >= lineAttackMoveDuration + 0.3) {
				finishAttack();
			}
			break;
		}
	}

	void TayamaBoss::generateLineAttack() {
		const double startY = Global::stageHeight + 32.0;
		for (double x = -lineAttackCherryInterval; x <= Global::stageWidth + lineAttackCherryInterval; x += lineAttackCherryInterval) {
			const Vec2 start{ x, startY };
			const Vec2 target{ x, lineAttackTargetY };
			bossStageManager->createCherry(createTrapLineCherry(start, target));
		}
	}

	std::shared_ptr<BossBarrageCherry> TayamaBoss::createTrapBarrageCherry() {
		auto cherry = std::make_shared<BossBarrageCherry>(pos, 1.0, BossCherryType::None);
		cherry->setCustomTexture(U"sprCherryTrap", 32, true);
		return cherry;
	}

	std::shared_ptr<BossSkyTargetCherry> TayamaBoss::createTrapSkyTargetCherry() {
		auto cherry = std::make_shared<BossSkyTargetCherry>(pos, 1.0, BossCherryType::None);
		cherry->setCustomTexture(U"sprCherryTrap", 32, true);
		return cherry;
	}

	std::shared_ptr<TayamaLineCherry> TayamaBoss::createTrapLineCherry(Vec2 startPos, Vec2 targetPos) {
		auto cherry = std::make_shared<TayamaLineCherry>(startPos, targetPos, 1.0, lineAttackMoveDuration);
		cherry->setCustomTexture(U"sprCherryTrap", 32, true);
		return cherry;
	}

	void TayamaBoss::finishAttack() {
		textureAngle = 0.0;
		applyScaleMag(baseScaleMag);
		attackStopwatch.reset();
		attackIntervalStopwatch.restart();
		attackCountInSet = (attackCountInSet + 1) % 4;
		appearanceStep = 2;
	}

	void TayamaBoss::updateDefeatedFall() {
		defeatedFallSpeed += defeatedFallAcceleration;
		pos.y += defeatedFallSpeed;
		textureAngle += defeatedRotateSpeed * Scene::DeltaTime();

		if (pos.y > Global::stageHeight + 128.0 * scaleMag) {
			isDelete = true;
		}
	}

	void TayamaBoss::barrageUpdate() {
		if (isDefeatedFall) {
			updateDefeatedFall();
			return;
		}

		switch (appearanceStep) {
		case 0:
			if (getIsMoveFinished()) {
				appearanceStep = 1;
			}
			break;
		case 1:
			if (hpBarAlpha < 1.0) {
				hpBarAlpha = Min(1.0, hpBarAlpha + 0.05);
			}
			else {
				attackIntervalStopwatch.restart();
				appearanceStep = 2;
			}
			hpBarDelay.update(hp, maxHp);
			break;
		case 2:
			hpBarDelay.update(hp, maxHp);
			if (attackIntervalStopwatch.sF() >= attackWaitTime) {
				startRandomAttack();
				appearanceStep = 3;
			}
			break;
		case 3:
			hpBarDelay.update(hp, maxHp);
			switch (currentAttackPattern) {
			case 0:
				updateRotatingSpreadAttack();
				break;
			case 1:
				updateTargetAttack();
				break;
			case 2:
				updateLineAttack();
				break;
			default:
				updateRotatingSpreadAttack();
				break;
			}
			break;
		}
	}

	void TayamaBoss::draw() const {
		constexpr int32 frameSize = 128;
		const int32 texRange = Periodic::Square0_1(0.5) * frameSize;
		TextureAsset(U"sprCherryTrapBoss")(texRange, 0, frameSize, frameSize)
			.scaled(scaleMag)
			.rotated(Math::ToRadians(textureAngle))
			.drawAt(pos, ColorF(1.0, isMuteki ? 0.6 : 1.0));
		//hitBox->draw(ColorF(0.5,0.5));

		if (!hasHp) {
			return;
		}

		const double width = Global::stageWidth;
		const double height = 20.0;
		const Vec2 barPos{ Global::stageWidth / 2.0, 0.0 };
		drawBossHpBar(barPos, width, height, hp, maxHp, hpBarAlpha, hpBarDelay);

		const String bossName = U"Boss : Tayama";
		const Vec2 textPos{ 6, 18 };
		for (const Vec2 offset : { Vec2{-1, 0}, Vec2{1, 0}, Vec2{0, -1}, Vec2{0, 1} }) {
			FontAsset(U"BossHp")(bossName).draw(textPos + offset, ColorF{ 0.0, 0.0, 0.0, hpBarAlpha });
		}
		FontAsset(U"BossHp")(bossName).draw(textPos, ColorF{ 1.0, 1.0, 1.0, hpBarAlpha });
	}

	void TayamaBoss::hited() {
		if (isMuteki || hp <= 0 || isDefeatedFall) {
			return;
		}

		Sound::playOneShot(Sound::BOSSHIT);
		--hp;

		if (hp <= 0) {
			Sound::playOneShot(Sound::DEATH);
			Sound::playOneShot(Sound::VC_BIKKURI);
			canPlayerKill = false;
			hasHp = false;
			isDefeatedFall = true;
			defeatedFallSpeed = 0.0;
			attackStopwatch.reset();
			attackIntervalStopwatch.reset();
			isMuteki = false;
			Global::isBossDefeated = true;
			return;
		}

		isMuteki = true;
		mutekiInterval.restart();
	}

	LowBossCherry::LowBossCherry(Vec2 startPos, double scale, BossStageManager& manager)
		: bossStageManager(&manager), Cherry(startPos, scale) {
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, 16.0 * scaleMag);
		type = ObjectType::Cherry;
		cherryType = CherryType::Boss;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = false;
		hasHp = true;
		maxHp = 25;
		hp = maxHp;
		hpBarDelay.reset(hp, maxHp);
		speed = moveSpeed;
		direction = 90;
		depth = 51;
		appearanceStep = 0;
	}

	void LowBossCherry::updateDefeatedFall() {
		defeatedFallSpeed += defeatedFallAcceleration;
		pos.y += defeatedFallSpeed;
		textureAngle += defeatedRotateSpeed * Scene::DeltaTime();

		if (pos.y > Global::stageHeight + 64.0 * scaleMag) {
			isDelete = true;
		}
	}

	void LowBossCherry::createSpreadAttack() {
		if (!bossStageManager) {
			return;
		}

		bossStageManager->createCherrySpread(spreadCherryNum, spreadCherrySpeed, [this]() { return createLowBossBarrageCherry(); });
	}

	void LowBossCherry::createTargetAttack() {
		if (!bossStageManager) {
			return;
		}

		bossStageManager->createSkyTargetCherry(targetLineNum, targetIsAddLine, [this]() { return createLowBossTargetCherry(); }, targetBaseSpeed, targetIntervalSpeed);
	}

	std::shared_ptr<BossBarrageCherry> LowBossCherry::createLowBossBarrageCherry() {
		auto cherry = std::make_shared<BossBarrageCherry>(pos, 1.0, BossCherryType::None);
		cherry->setCustomAppleEmoji();
		return cherry;
	}

	std::shared_ptr<BossSkyTargetCherry> LowBossCherry::createLowBossTargetCherry() {
		auto cherry = std::make_shared<BossSkyTargetCherry>(pos, 1.0, BossCherryType::None);
		cherry->setCustomAppleEmoji();
		return cherry;
	}

	void LowBossCherry::barrageUpdate() {
		if (isDefeatedFall) {
			updateDefeatedFall();
			return;
		}

		switch (appearanceStep) {
		case 0:
			if (pos.y <= targetY) {
				pos.y = targetY;
				speed = 0.0;
				appearanceStep = 1;
				spreadStopwatch.restart();
				targetStopwatch.restart();
				createSpreadAttack();
				createTargetAttack();
			}
			break;
		case 1:
			if (spreadStopwatch.sF() >= spreadInterval) {
				createSpreadAttack();
				spreadStopwatch.restart();
			}
			if (targetStopwatch.sF() >= targetInterval) {
				createTargetAttack();
				targetStopwatch.restart();
			}
			break;
		}

		if (hpBarAlpha < 1.0) {
			hpBarAlpha = Min(1.0, hpBarAlpha + 0.05);
		}
		hpBarDelay.update(hp, maxHp);
	}

	void LowBossCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		static const Texture appleTexture{ U"\U0001F34E"_emoji };

		appleTexture
			.scaled(0.25 * scaleMag)
			.rotated(Math::ToRadians(textureAngle))
			.drawAt(pos, ColorF{ 1.0, isMuteki ? 0.6 : 1.0 });

		//hitBox->draw(ColorF{ 0.2, 0.7, 1.0, 0.35 });

		if (!hasHp) {
			return;
		}

		const double width = Global::stageWidth;
		const double height = 20.0;
		const Vec2 barPos{ Global::stageWidth / 2.0, 0.0 };
		drawBossHpBar(barPos, width, height, hp, maxHp, hpBarAlpha, hpBarDelay);

		const String bossName = U"Boss";
		const Vec2 textPos{ 6, 18 };
		for (const Vec2 offset : { Vec2{-1, 0}, Vec2{1, 0}, Vec2{0, -1}, Vec2{0, 1} }) {
			FontAsset(U"BossHp")(bossName).draw(textPos + offset, ColorF{ 0.0, 0.0, 0.0, hpBarAlpha });
		}
		FontAsset(U"BossHp")(bossName).draw(textPos, ColorF{ 1.0, 1.0, 1.0, hpBarAlpha });
	}

	void LowBossCherry::hited() {
		if (isMuteki || hp <= 0 || isDefeatedFall) {
			return;
		}

		Sound::playOneShot(Sound::BOSSHIT);
		--hp;

		if (hp <= 0) {
			Sound::playOneShot(Sound::DEATH);
			canPlayerKill = false;
			hasHp = false;
			isDefeatedFall = true;
			defeatedFallSpeed = 0.0;
			isMuteki = false;
			Global::isBossDefeated = true;
			if (Global::moraleValue1 < 30 && Global::moraleValue2 < 30) {
				Global::endingValue = 1;
			}
			return;
		}

		isMuteki = true;
		mutekiInterval.restart();
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
		hpBarDelay.reset(hp, maxHp);

		speed = 0;
		direction = 0;
		gravity = 0;

		startStep = 0;

		r = 0;
		rMax = 85;
		c = static_cast<double>(cType) * 60;
	}

	void BossSubCherry::barrageUpdate() {

		if (Global::isBossDefeated) {
			alpha = 0;
			setTypeColor();
			return;
		}

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
		hpBarDelay.update(hp, maxHp);
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

			drawBossHpBar(barPos, width, height, hp, maxHp, 1.0, hpBarDelay);
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
		if (customAppleEmoji) {
			static const Texture appleTexture{ U"\U0001F34E"_emoji };
			appleTexture.scaled(0.13 * scaleMag).drawAt(pos.x - 1, pos.y - 1, ColorF{ 1.0, alpha });
			//hitBox->draw(ColorF{ 0.2, 0.7, 1.0, 0.35 });
			return;
		}
		if (customTextureName != U"") {
			const int32 texRange = customTextureAnimation ? (static_cast<int32>(Periodic::Square0_1(0.5)) * customTextureEdge) : 0;
			TextureAsset(customTextureName)(texRange, 0, customTextureEdge, customTextureEdge).scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1, ColorF(customTextureColor.r, customTextureColor.g, customTextureColor.b, customTextureColor.a * alpha));
			return;
		}
		TextureAsset(U"sprCherryLowBarrageWhite").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1, typeColor);
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}

	void BossBarrageCherry::setCustomTexture(String textureName, int32 textureEdge, bool hasAnimation) {
		customTextureName = textureName;
		customTextureEdge = textureEdge;
		customTextureAnimation = hasAnimation;
		customAppleEmoji = false;
	}

	void BossBarrageCherry::setCustomAppleEmoji() {
		customAppleEmoji = true;
		customTextureName = U"";
		hitBox = std::make_shared<CircleHitBox>(pos, 9.0 * scaleMag);
	}

	void BossBarrageCherry::setCustomTextureColor(ColorF color) {
		customTextureColor = color;
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
		if (!Global::isBossExBarrageAttack) {
			//攻撃強化状態前
			if (!Global::isBossAttackPowerUp) {
				switch (cherrySubType) {
				case BossCherryType::Red:
					bossStageManager->createCherrySpread(25, 6, [this]() { return std::make_shared<BossBarrageCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::BLOCKBREAK);
					break;
				case BossCherryType::Blue:
					bossStageManager->createBlueLineCherry(20, 80, [this]() { return std::make_shared<BossFallBlueCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::CHERRYFALL);
					break;
				case BossCherryType::Yellow:
					bossStageManager->createYellowStarCherry(5, 2, pos, 7, [this]() { return std::make_shared<BossYellowStarCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::BLOCKBREAK);
					break;
				case BossCherryType::Green:
					bossStageManager->createGreenWaveCherry(pos, 0.05, 10, [this]() { return std::make_shared<BossGreenWaveCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::BLOCKBREAK);
					break;
				case BossCherryType::Orange:
					bossStageManager->createOrangeStopCherry(false, [this]() { return std::make_shared<BossOrangeStopCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::SPIKETRAP);
					break;
				case BossCherryType::Sky:
					bossStageManager->createSkyTargetCherry(7, false, [this]() { return std::make_shared<BossSkyTargetCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::BLOCKBREAK);
					break;
				}
			}
			else {
				switch (cherrySubType) {
				case BossCherryType::Red:
					bossStageManager->createCherrySpread(20, 6, [this]() { return std::make_shared<BossBarrageCherry>(pos, 1.0, cherrySubType); });
					bossStageManager->createCherrySpread(25, 4, [this]() { return std::make_shared<BossBarrageCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::BLOCKBREAK);
					break;
				case BossCherryType::Blue:
					bossStageManager->createBlueLineCherry(40, 50, [this]() { return std::make_shared<BossFallBlueCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::CHERRYFALL);
					break;
				case BossCherryType::Yellow:
					bossStageManager->createYellowStarCherry(5, 2, pos, 10, [this]() { return std::make_shared<BossYellowStarCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::BLOCKBREAK);
					break;
				case BossCherryType::Green:
					bossStageManager->createGreenWaveCherry(pos, 0.05, 13, [this]() { return std::make_shared<BossGreenWaveCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::BLOCKBREAK);
					break;
				case BossCherryType::Orange:
					bossStageManager->createOrangeStopCherry(true, [this]() { return std::make_shared<BossOrangeStopCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::SPIKETRAP);
					break;
				case BossCherryType::Sky:
					bossStageManager->createSkyTargetCherry(5, true, [this]() { return std::make_shared<BossSkyTargetCherry>(pos, 1.0, cherrySubType); });
					Sound::playOneShot(Sound::BLOCKBREAK);
					break;
				}
			}
		}
		else {
			//Ex用
			switch (cherrySubType) {
			case BossCherryType::Red:
				bossStageManager->createCherrySpread(20, 6, [this]() { return std::make_shared<BossBarrageCherry>(pos, 1.0, cherrySubType); });
				Sound::playOneShot(Sound::BLOCKBREAK);
				break;
			case BossCherryType::Blue:
				bossStageManager->createBlueLineCherry(20, 80, [this]() { return std::make_shared<BossFallBlueCherry>(pos, 1.0, cherrySubType); });
				Sound::playOneShot(Sound::CHERRYFALL);
				break;
			case BossCherryType::Yellow:
				bossStageManager->createYellowStarCherry(5, 2, pos, 6, [this]() { return std::make_shared<BossYellowStarCherry>(pos, 1.0, cherrySubType); });
				Sound::playOneShot(Sound::BLOCKBREAK);
				break;
			case BossCherryType::Green:
				bossStageManager->createGreenWaveCherry(pos, 0.05, 11, [this]() { return std::make_shared<BossGreenWaveCherry>(pos, 1.0, cherrySubType); });
				Sound::playOneShot(Sound::BLOCKBREAK);
				break;
			case BossCherryType::Orange:
				bossStageManager->createOrangeStopCherry(false, [this]() { return std::make_shared<BossOrangeStopCherry>(pos, 1.0, cherrySubType); });
				Sound::playOneShot(Sound::SPIKETRAP);
				break;
			case BossCherryType::Sky:
				bossStageManager->createSkyTargetCherry(6, false, [this]() { return std::make_shared<BossSkyTargetCherry>(pos, 1.0, cherrySubType); });
				Sound::playOneShot(Sound::BLOCKBREAK);
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
