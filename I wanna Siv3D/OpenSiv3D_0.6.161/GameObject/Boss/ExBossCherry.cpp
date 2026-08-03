#include "ExBossCherry.h"
#include "../../Audio/AudioAsset.h"
#include "../../StageManager/BossStageManager.h"

namespace Iwanna {
	ExBossCherry::ExBossCherry(Vec2 startPos, double scale, BossStageManager& manager) : bossStageManager(&manager), Cherry(startPos, scale) {

		//GameObject.hの値初期化
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);
		type = ObjectType::Cherry;
		cherryType = CherryType::ExBoss;
		nowAttackType = ExBossAttackType::None;

		sordCherriesManager = bossStageManager->getExBossSordManagerCherry().get();

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = false;

		hasHp = true;
		maxHp = 60;
		hp = maxHp;
		hpBarDelay.reset(hp, maxHp);
		bossForm = BossForm::First;

		baseCenterPos = Vec2{ 800, 330 };
		speed = 20;
		direction = 90;
		gravity = 0;

		depth = 50;

		c = 0;
		r = 20;
		waitTime = 1.0;
		startStep = 0;
	}

	void ExBossCherry::barrageUpdate() {

		//ボス戦開始時の処理
		switch (startStep) {
		case 0:
				isNowAttacking = true;
				pos.x = 800;
				pos.y = -300;
				speed = 0;
				movePosition(Vec2{ 800, 330 }, 1.8, false);
				rotateDirection(140, 1.8, false);
				startStep++;
			break;
		case 1:
			if (getIsMoveFinished() && getIsRotateFinished()) {
				movePosition(Vec2{ 800, 290 }, 0.2, false);//抜刀
				//playerのいる方向に回転
				if(isPlayerInRightSide)rotateDirection(-180, 0.6, false);
				else rotateDirection(180, 0.6, false);

				Sound::playOneShot(Sound::SORD_STRONG);
				sordCherriesManager->startFollowBoss();
				sordCherriesManager->setSordCanPlayerKill(true);
				startStep++;
			}
			break;
		case 2://少し待機
			if (getIsMoveFinished() && getIsRotateFinished()) {
				rotateDirection(0, 0.4, false);
				startStep++;
			}
			break;
		case 3:
			if (getIsRotateFinished()) {
				baseAngle = textureAngle;
				sordCherriesManager->setSordCanPlayerKill(false);
				nowAttackType = ExBossAttackType::SparkExpro;
				startStep++;
			}
			break;
		}

		//player関連の情報を取得
		playerPos = bossStageManager->getPlayer()->pos;
		isPlayerInRightSide = playerPos.x > pos.x;
		playerDistance = calculateDistance(pos, playerPos);

		//hp表示のフェードイン
		if (hpBarAlpha < 1)hpBarAlpha += 0.05;
		hpBarDelay.update(hp, maxHp);

		updateBossForm();
		
		//攻撃処理
		attack();

		//剣の状態を更新
		sordCherriesManager->setExBossPos(pos);
		sordCherriesManager->setExBossAngle(textureAngle);
	}

	void ExBossCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLowBoss").scaled(scaleMag).rotated(Math::ToRadians(textureAngle)).drawAt(pos.x - 1, pos.y - 1, ColorF(1.0, isMuteki ? 0.6 : 1.0));

		// ===== HPバー =====
		if (hasHp) {
			double width = Global::windowWidth;   // 横幅
			double height = 20;              // 高さ
			Vec2 barPos = Vec2{ bossStageManager->executeCameraPos().x,0 };

			drawBossHpBar(barPos, width, height, hp, maxHp, hpBarAlpha, hpBarDelay);

			// 文字表示
			Vec2 textBasePos = barPos + Vec2(-380, 18);
			FontAsset(U"BossHp")(U"Guardian Cherry , the Sword Saint").draw(textBasePos.x - 1, textBasePos.y, ColorF(0, 0, 0, hpBarAlpha));
			FontAsset(U"BossHp")(U"Guardian Cherry , the Sword Saint").draw(textBasePos.x + 1, textBasePos.y, ColorF(0, 0, 0, hpBarAlpha));
			FontAsset(U"BossHp")(U"Guardian Cherry , the Sword Saint").draw(textBasePos.x, textBasePos.y - 1, ColorF(0, 0, 0, hpBarAlpha));
			FontAsset(U"BossHp")(U"Guardian Cherry , the Sword Saint").draw(textBasePos.x, textBasePos.y + 1, ColorF(0, 0, 0, hpBarAlpha));

			// 本体（白）
			FontAsset(U"BossHp")(U"Guardian Cherry , the Sword Saint").draw(textBasePos.x, textBasePos.y, ColorF(1.0, 1.0, 1.0, hpBarAlpha));
		}
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}

	double ExBossCherry::getBaseAngleDiff() const {
		return baseAngle - textureAngle;
	}

	//bossの攻撃を設定
	void ExBossCherry::decideAttack() {
		nowAttackType = canAttackTypes.choice();
	}

	//ダメージを受けた際の処理
	void ExBossCherry::hited() {
		if (hp > 0) {
			Sound::playOneShot(Sound::BOSSHIT);
			hp--;
		}

		if (hp <= 0) {
			Sound::playOneShot(Sound::DEATH);
			throw Error{ U"おめでとう！君はボスを撃破した！" };
			Global::isBossDefeated = true;
		}

		isMuteki = true;
		mutekiInterval.restart();
	}

	//bossの攻撃形態を設定
	void ExBossCherry::updateBossForm() {
		if (hp > 50)bossForm = BossForm::First;
		else if (hp > 35)bossForm = BossForm::Second;
		else if (hp > 15)bossForm = BossForm::Third;
		else bossForm = BossForm::Forth;
	}

	//引数の確率でtrueを返す関数
	bool ExBossCherry::getRandomChance(double p) {
		p = Clamp(p, 0.0, 1.0);
		return Random() < p;
	}

	// --- 剣を構成するりんご --- //
	SordCherry::SordCherry(Vec2 startPos, double scale, CherryColorType colorType) : BarrageCherry(startPos, scale, colorType) {
		pos = startPos;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);

		canPlayerKill = false;
		isDeleteOutOfScreen = false;
		alpha = 1.0;
		hasAnimation = false;
		cherryTextureName = U"sprCherryLowAllWhite";
		depth = 18;

		alpha = 0.0;

		cherryColorType = colorType;
		setTypeColor();
	}

	void SordCherry::barrageUpdate() {
		switch (startStep) {
		case 0:
			setSordRotateStatus(sordBaseCenterPos);
			c += 180;
			startStep++;
			break;
		case 1:
			if (!isFollowBoss) exBossAngle = 0;
			pos.x = r * cos(Math::ToRadians(c + exBossAngle)) + sordBaseCenterPos.x;
			pos.y = r * sin(Math::ToRadians(c + exBossAngle)) + sordBaseCenterPos.y;
			break;
		}

		//刃のきらめき処理
		if (sordCherryType == SordCherryType::Blade && canSpark) {
			switch (sparkStep) {
			case 0:
				if (sparkStopwatch.sF() > startSparkTime) {
					sparkTimer.start();
					sparkStopwatch.reset();
					sparkStep++;
				}
				break;
			case 1:
				sparkAlpha = sparkTimer.progress0_1();
				if (sparkTimer.reachedZero()) {
					sparkTimer.restart();
					sparkStep++;
				}
				break;
			case 2:
				sparkAlpha = sparkTimer.progress1_0();
				if (sparkTimer.reachedZero()) {
					sparkTimer.reset();
					sparkStep++;
				}
				break;
			}
		}

		setTypeColor();
	}

	void SordCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(cherryTextureName)(0, 0, textureEdge, textureEdge).scaled(scaleMag).drawAt(pos.x, pos.y - 1, typeColor);

		//spark部分
		TextureAsset(cherryTextureName)(0, 0, textureEdge, textureEdge).scaled(scaleMag).drawAt(pos.x, pos.y - 1, ColorF(1.0,sparkAlpha));
		//hitBox->draw(Palette::Blue);//判定の可視化
	}

	void SordCherry::setSordBaseCenterPos(Vec2 pos) {
		sordBaseCenterPos = pos;
	}

	// 剣型りんごの回転の基準を変える際に利用
	void SordCherry::setSordRotateStatus(Vec2 basePos) {
		setSordBaseCenterPos(basePos);
		r = calculateDistance(pos, basePos);
		calculateDirection(pos, basePos);
		c = direction;
		alpha = 1.0;
	}

	// ExBossのtexture角度を取得する
	void SordCherry::setExBossAngle(double c) {
		exBossAngle = c - exBossAngleOffset;
	}

	void SordCherry::setIsFollowBoss(bool bl) {
		isFollowBoss = bl;
	}

	//刃の輝きを可能にする前の初期化処理
	void SordCherry::initSparking(double time) {
		startSparkTime = time;
		canSpark = true;
		sparkStep = 0;
		sparkStopwatch.restart();
	}

	// --- 剣の判定用りんご --- //
	SordHitBoxCherry::SordHitBoxCherry(Vec2 startPos, double scale, CherryColorType colorType) : SordCherry(startPos, scale, colorType) {
		pos = startPos;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);

		canPlayerKill = false;
		isDeleteOutOfScreen = false;
		alpha = 1.0;
		hasAnimation = false;
		cherryTextureName = U"sprCherryLowAllWhite";
		depth = 100;
	}

	void SordHitBoxCherry::draw() const {
		//hitBox->draw(ColorF(0.7, 0.7));//判定の可視化
	}

	// ----- 剣型りんごの動き制御用りんご ----- //
	SordCherriesManager::SordCherriesManager(Vec2 startPos, double scale, BossStageManager& manager) : bossStageManager(&manager), Cherry(startPos, scale) {
		pos = startPos;
		canPlayerKill = false;
		isDeleteOutOfScreen = false;

		alpha = 1.0;

		hasAnimation = false;
		cherryTextureName = U"sprCherryLowWhite";

		depth = 9;

		cherryType = CherryType::ExBoss;

		createSordCherries();
	}

	void SordCherriesManager::barrageUpdate() {
		for (const auto& cherry : sordCherries) {
			cherry->barrageUpdate();
		}

		if (isFollowBoss) {
			pos.x = r * cos(Math::ToRadians(c + exBossAngle)) + exBossPos.x;
			pos.y = r * sin(Math::ToRadians(c + exBossAngle)) + exBossPos.y;

			sordEdgePos.x = (r + 160) * cos(Math::ToRadians(c + exBossAngle)) + exBossPos.x;
			sordEdgePos.y = (r + 160) * sin(Math::ToRadians(c + exBossAngle)) + exBossPos.y;

			for (const auto& cherry : sordCherries) {

				//剣に攻撃判定がある場合は残像エフェクトを生成
				if(cherry->sordCherryType == SordCherryType::Hitbox && generateEffectTimer.reachedZero()) {
					const std::function<std::shared_ptr<Cherry>()>& effectCherry = [&]() {
						return std::make_shared<FadeCherry>(cherry->pos, 1.7, U"sprCherryLowAllWhite", CherryColorType::Red, 0.5);
					};
					bossStageManager->createCherry(effectCherry());
				}
			}

			if(generateEffectTimer.reachedZero()) {
				generateEffectTimer.restart();
			}
		}
	}

	void SordCherriesManager::setSordBaseCenterPos(Vec2 pos) {
		for (const auto& cherry : sordCherries) {
			cherry->setSordBaseCenterPos(pos);
		}
	}

	// ExBossの座標を取得する
	void SordCherriesManager::setExBossPos(Vec2 bossPos) {
		exBossPos = bossPos;

		if (isFollowBoss) {
			for (const auto& cherry : sordCherries) {
				cherry->setSordBaseCenterPos(exBossPos);
			}
		}
	}

	// ExBossのtexture角度を取得する
	void SordCherriesManager::setExBossAngle(double c) {
		exBossAngle = c - exBossAngleOffset;
		for (const auto& cherry : sordCherries) {
			cherry->setExBossAngle(c);
		}
	}

	//剣の判定用りんごの当たり判定の有効無効を設定する
	void SordCherriesManager::setSordCanPlayerKill(bool bl) {
		for (const auto& cherry : sordCherries) {
			if (cherry->sordCherryType == SordCherryType::Hitbox) {
				cherry->canPlayerKill = bl;
			}
		}

		if(bl) generateEffectTimer.restart();
		else generateEffectTimer.reset();
	}

	//剣の先端座標を取得する
	Vec2 SordCherriesManager::getSordEdgePos() {
		return sordEdgePos;
	}

	// 呼び出されると、ExBossへの追従を開始する
	void SordCherriesManager::startFollowBoss() {

		//自身の位置、回転に必要な情報設定
		offsetBoss = pos - exBossPos;
		r = calculateDistance(pos, exBossPos);
		calculateDirection(pos, exBossPos);
		c = direction;

		//剣を構成するりんごの設定
		for (const auto& cherry : sordCherries) {
			cherry->setSordRotateStatus(exBossPos);
			cherry->setIsFollowBoss(true);
		}

		isFollowBoss = true;
	}

	void SordCherriesManager::draw() const {
		for (const auto& cherry : sordCherries) {
			cherry->draw();
		}

		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		//TextureAsset(cherryTextureName)(0, 0, textureEdge, textureEdge).scaled(scaleMag).drawAt(pos.x, pos.y - 1, typeColor);
		//hitBox->draw(ColorF(0.7, 0.7));//判定の可視化
	}

	void SordCherriesManager::sparkSordBlade() {
		double sparkInterval = 0.02;
		int32 sparkBladeCount = 0;

		for (const auto& cherry : sordCherries) {
			if (cherry->sordCherryType == SordCherryType::Handle)break;

			cherry->initSparking(sparkInterval * sparkBladeCount);
			if (cherry->bladeId % 2 == 0)sparkBladeCount++;
		}
	}

	void SordCherriesManager::createSordCherries() {
		//ラムダ式で工場関数を定義
		//剣の刃を構成するりんご
		auto bladeCherryFactory = [this]() {
			return std::make_shared<SordCherry>(pos, 1.0, CherryColorType::Gray);
			};
		//剣の柄を構成するりんご
		auto handleCherryFactory = [this]() {
			return std::make_shared<SordCherry>(pos, 1.0, CherryColorType::Black);
			};
		//剣の柄を構成するりんご
		auto handleCherry2Factory = [this]() {
			return std::make_shared<SordCherry>(pos, 0.8, CherryColorType::Black);
			};

		// 刃部分の生成
		int bladeXnum = 2;
		int bladeYnum = 8;
		double bladeInterval = 16.0;
		Vec2 bladeStartPos = Vec2{ pos.x - (bladeInterval * (bladeXnum - 1) / 2), pos.y };
		int32 bladeCount = 0;
		for (int j = 0; j <= bladeYnum; j++) {
			for (int i = 0; i < bladeXnum; i++) {
				auto cherry = bladeCherryFactory();
				if (j < bladeYnum) {
					cherry->pos.x = bladeStartPos.x + i * bladeInterval;
					cherry->pos.y = bladeStartPos.y - j * bladeInterval;
					cherry->setSordBaseCenterPos(pos);
					cherry->sordCherryType = SordCherryType::Blade;
					cherry->bladeId = bladeCount;
					sordCherries.push_back(cherry);
				}
				else {
					cherry->pos.x = pos.x;
					cherry->pos.y = bladeStartPos.y - j * bladeInterval;
					cherry->setSordBaseCenterPos(pos);
					cherry->sordCherryType = SordCherryType::Blade;
					cherry->bladeId = bladeCount;
					sordCherries.push_back(cherry);
				}
				bladeCount++;
			}
		}

		// 刃の判定用のりんごの生成
		int hitBoxNum = 4;
		double bladeHitBoxInterval = 36.0;
		for(int i = 0; i < hitBoxNum; i++) {
			auto cherry = std::make_shared<SordHitBoxCherry>(pos, 2.0, CherryColorType::Gray);
			cherry->pos.x = pos.x;
			cherry->pos.y = bladeStartPos.y - i * bladeHitBoxInterval;
			cherry->setSordBaseCenterPos(pos);
			cherry->sordCherryType = SordCherryType::Hitbox;
			sordCherries.push_back(cherry);
			bossStageManager->createCherry(cherry);
		}

		// 柄部分の生成
		int handleXnum = 2;
		int handleYnum = 7;
		double handleInterval = 10.0;
		Vec2 handleStartPos = Vec2{ pos.x - (handleInterval * (handleXnum - 1) / 2), pos.y };
		for (int j = 0; j < handleYnum; j++) {
			for (int i = 0; i < handleXnum; i++) {
				auto cherry = handleCherry2Factory();
				if (j < handleYnum) {
					cherry->pos.x = handleStartPos.x + i * handleInterval;
					cherry->pos.y = handleStartPos.y + j * handleInterval;
					cherry->setSordBaseCenterPos(pos);
					cherry->sordCherryType = SordCherryType::Handle;
					sordCherries.push_back(cherry);
				}
			}
		}

		// 柄部分(黒)の生成
		handleXnum = 6;
		handleYnum = 2;
		handleInterval = 14.0;
		handleStartPos = Vec2{ pos.x - (handleInterval * (handleXnum - 1) / 2), pos.y };
		for (int j = 0; j < handleYnum; j++) {
			for (int i = 0; i < handleXnum; i++) {
				auto cherry = handleCherryFactory();
				if (j < handleYnum) {
					cherry->pos.x = handleStartPos.x + i * handleInterval;
					cherry->pos.y = handleStartPos.y + j * handleInterval;
					cherry->setSordBaseCenterPos(pos);
					cherry->sordCherryType = SordCherryType::Handle;
					sordCherries.push_back(cherry);
				}
			}
		}
	}

	// --- 爆破時のりんご --- //
	ExproCherry::ExproCherry(Vec2 startPos, double scale) : Cherry(startPos, scale) {
		pos = startPos;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);

		canPlayerKill = true;
		isDeleteOutOfScreen = false;
		hasAnimation = false;

		cherryTextureName = U"sprCherryLowWhite";
		cherryColorType = CherryColorType::Gray;
		depth = 18;

		alpha = 1.0;
		scaleMag = 1.0;

		attackTimer.start();

		setTypeColor();
	}

	void ExproCherry::barrageUpdate() {

		if (step == 0) {
			startSpeed = speed;
			step++;
		}

		if (attackTimer.isRunning()) {
			alpha = EaseOutQuint(attackTimer.progress1_0());
			speed = EaseOutQuart(attackTimer.progress1_0()) * startSpeed;
		}
		else {
			isDelete = true;
		}

		setTypeColor();
	}
}
