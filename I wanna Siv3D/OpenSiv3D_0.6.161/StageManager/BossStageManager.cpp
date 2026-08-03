#include "BossStageManager.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	BossStageManager::BossStageManager() {
		stockNearGameObjects.cellSize = 96;
		stockBulletsNearGameObjects.cellSize = 320;
		stockLargeNearGameObjects.cellSize = 800;
	}

	void BossStageManager::setUpObjects(int32 chapter) {
		gameObjects.player = std::make_shared<Player>();

		// 既存のオブジェクトを抹消して初期化
		gameObjects.bullets.clear();
		gameObjects.cherries.clear();
		gameObjects.bossCherries.clear();
		gameObjects.blocks.clear();
		gameObjects.spikes.clear();
		gameObjects.savePoints.clear();
		gameObjects.bloods.clear();
		gameObjects.warps.clear();

		// 一部変数の初期化
		isGenerateBloods = false;

		Global::trap2MapBgmStop = false;
		Global::trapActivatedId30InTrap2Map = false;
		Global::trapCameraActivatedInTrap2Map = false;
		Global::isPlayerFrozen = false;
		Global::isBossAttackPowerUp = false;
		Global::isBossExBarrageAttack = false;
		Global::isBossDefeated = false;
		Global::isCameraFollowMode = false;
		isTrapBossSecondPhaseIntroStarted = false;
		isTrapBossSecondPhaseStarted = false;
		isTrapBossSecondPhaseDefeated = false;
		isTrapBossSecondPhaseDefeatedFall = false;
		hasTrapBossSecondPhaseBrokenBlocks = false;
		trapBossSecondPhaseHp = trapBossSecondPhaseMaxHp;
		trapBossSecondPhaseHpBarDelay.reset(trapBossSecondPhaseHp, trapBossSecondPhaseMaxHp);
		trapBossSecondPhaseTayamaCenterPos = Vec2{ 370, 304 };
		trapBossSecondPhaseTayamaAngle = 0.0;
		trapBossSecondPhaseDefeatedFallSpeed = 0.0;
		isTrapBossSecondPhaseEyeHitFlash = false;
		trapBossSecondPhaseEyeAttackCount = 0;
		trapBossSecondPhaseTargetAttackCount = 0;
		trapBossSecondPhaseDarkAlpha = trapBossSecondPhaseDarkAlphaMax;
		trapBossSecondPhaseIntroStopwatch.reset();
		trapBossSecondPhaseEyeHitFlashStopwatch.reset();
		trapBossSecondPhaseAttackStopwatch.reset();
		trapBossSecondPhaseTargetAttackStopwatch.reset();

		gameoverTimer.reset();
		isShowGameOver = false;

		titleCard.reset();

		if(Global::isChangeRoom)loadGameObjects(Global::nowRoomName);
		else loadGameObjects(Global::savedRoomName);
		Global::isChangeRoom = false;

		// shared_ptr をコールバックへ取り込むと循環参照になるため、
		// 生存期間が同じセーブポイントのポインタを一度だけ設定する
		for (auto& savePoint : gameObjects.savePoints) {
			auto* savePointPtr = savePoint.get();
			savePoint->onSavedCallback = [this, savePointPtr]() {
				generateBoss(savePointPtr->getAppendBossId());
				saveGame();
			};
		}
	}

	void BossStageManager::loadGameObjects(String fileName) {

		String quarity;

		switch (Global::mainTextureNumber) {
		case 0: quarity = U"low"; break;
		case 1: quarity = U"normal"; break;
		}

		//ステージデータの読み込みとオブジェクト生成
		CSV csv{ U"MapData/" + fileName + U".csv"};

		if (!csv)
		{
			throw Error{ U"次のCSVが読み込めません : " + fileName + U".csv"};
		}

		for (size_t y = 0; y < csv.rows(); ++y)
		{
			for (size_t x = 0; x < csv.columns(y); ++x)
			{
				int value = csv.get<int>(y, x);

				// 0 は空白として無視
				if (value == 0)
					continue;

				Vec2 pos{
					x,
					y
				};

				// value に応じて配置
				switch (value) {
				case 1: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_" + quarity + U"1", pos); break;
				case 6: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_" + quarity + U"2", pos); break;
				case 7: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_" + quarity + U"3", pos); break;
				case 21: gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 0); break;
				case 22: gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 1); break;
				case 23: gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 2); break;
				case 24: gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 3); break;
				//case 25: gameObjects.savePoints << std::make_shared<SavePoint>(pos); break;
				case 26: gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_" + quarity + U"1", pos); break;
				case 27: gameObjects.blocks << std::make_shared<ShootTroughBlock>(U"sprBlockShootTrough", pos); break;
				case 28: gameObjects.blocks << std::make_shared<FakeBlock>(U"sprBlock_" + quarity + U"2", pos); break;
				}
			}
		}

		//ステージサイズを更新
		Global::stageWidth = csv.columns(0) * oneTileSize;
		Global::stageHeight = csv.rows() * oneTileSize;

		stageName = fileName;

		// 次に同名のJsonファイルからギミックデータを読み込む

		const String jsonPath = U"MapData/" + fileName + U".json";
		const auto& jsonArray = JSON::Load(jsonPath);

		// ロードに失敗した場合はエラーを投げる
		if (not jsonArray) {
			throw Error{ U"対応するJSONファイルをロードできませんでした : " + jsonPath };
		}

		// ステージ情報をパース
		for (const auto& stage : jsonArray.arrayView()) {
			// プレイヤーの初期位置を取得し反映
			Vec2 startPlayerPos = parsePos(stage[U"startPlayerPos"]);


			startPlayerPos *= oneTileSize;
			// セーブデータが無い場合、初期位置をCSVの値から設定
			if (!Global::isExistSaveData || Global::isChangeRoom) {
				gameObjects.player->pos = startPlayerPos;
			}
			else {
				gameObjects.player->pos = Global::savedStartPlayerPos;
			}

			//各ギミックの情報を取得し反映
			String gimmikName;
			Vec2 gimmikParsePos;
			Vec2 gimmikIntactPos;
			double gimmikValue1;
			double gimmikValue2;
			double gimmikValue3;
			double gimmikValue4;
			String gimmikString;

			if (stage.contains(U"Gimmiks")) {
				for (const auto& gimmik : stage[U"Gimmiks"].arrayView()) {
					gimmikName = gimmik[U"gimmikName"].getString();
					gimmikParsePos = parsePos(gimmik[U"gimmikPos"]);
					gimmikIntactPos = parseIntactPos(gimmik[U"gimmikPos"]);

					if (gimmikName == U"ワープ" || gimmikName == U"特殊ワープ") {
						gimmikString = gimmik[U"value1"].getString();
					}
					else {
						if (gimmik.contains(U"value1")) gimmikValue1 = gimmik[U"value1"].get<double>();
						if (gimmik.contains(U"value2")) gimmikValue2 = gimmik[U"value2"].get<double>();
						if (gimmik.contains(U"value3")) gimmikValue3 = gimmik[U"value3"].get<double>();
						if (gimmik.contains(U"value4")) gimmikValue4 = gimmik[U"value4"].get<double>();
					}

					if (gimmikName == U"罠ブロック") gameObjects.blocks << std::make_shared<BreakBlock>(U"sprBlock_" + quarity + U"3", gimmikParsePos, static_cast<int32>(gimmikValue1));
					if (gimmikName == U"時間罠ブロック") gameObjects.blocks << std::make_shared<TimedBreakBlock>(U"sprBlock_" + quarity + U"3", gimmikParsePos, static_cast<int32>(gimmikValue1), gimmikValue2);
					if (gimmikName == U"ワープ") gameObjects.warps << std::make_shared<Warp>(gimmikIntactPos, gimmikString);
					if (gimmikName == U"ループ移動針") gameObjects.spikes << std::make_shared<SpikeLoopMove>(quarity, gimmikIntactPos, static_cast<int32>(gimmikValue1), Vec2{ gimmikValue2, gimmikValue3 }, gimmikValue4);
				}
			}

			//背景ロード
			if (stage.contains(U"background")) {
				backgroundName = U"background_" + stage[U"background"].getString();
			}
			else {
				backgroundName = U"background_sample";
			}
		}

		if (stageName == U"boss") {
			gameObjects.savePoints << std::make_shared<BossSavePoint>(Vec2{400,500},1);
		}
		if (stageName == U"bossLow") {
			gameObjects.savePoints << std::make_shared<BossSavePoint>(Vec2{ 400,500 }, 4);
		}
		if (stageName == U"ExBoss") {
			gameObjects.savePoints << std::make_shared<BossSavePoint>(Vec2{ 800,450 }, 2);
			gameObjects.bossCherries << std::make_shared<SordCherriesManager>(Vec2{ 800,430 }, 2.0, *this);
			Global::isCameraFollowMode = true;
			Global::isBossExBarrageAttack = true;
		}
		if (stageName == U"trapBoss") {
			gameObjects.savePoints << std::make_shared<BossSavePoint>(Vec2{ 400,500 }, 3);
		}
	}

	Vec2 BossStageManager::parsePos(const JSON& json) {
		return Vec2{ json[0].get<int32>() / oneTileSize, json[1].get<int32>() / oneTileSize };
	}

	Vec2 BossStageManager::parseIntactPos(const JSON& json) {
		return Vec2{ json[0].get<int32>(), json[1].get<int32>()};
	}

	void BossStageManager::update() {

		// ----- update関連 -----
		auto& player = gameObjects.player;
		auto& bullets = gameObjects.bullets;
		auto& cherries = gameObjects.cherries;
		auto& bossCherries = gameObjects.bossCherries;
		auto& blocks = gameObjects.blocks;
		auto& spikes = gameObjects.spikes;
		auto& savePoints = gameObjects.savePoints;
		auto& bloods = gameObjects.bloods;
		auto& warps = gameObjects.warps;

		//死亡判定
		if (player->getIsDead()) {
			gameoverTimer.start();
			if (gameoverTimer.reachedZero()) {
				isShowGameOver = true;
			}
		}

		//タイトルカード処理
		titleCard.update();

		// 揺れ更新
		cameraShake.update();
		updateTrapBossSecondPhaseIntro();
		updateTrapBossSecondPhaseDefeatedFall();
		updateTrapBossSecondPhaseEyeAttack();
		updateTrapBossSecondPhaseTargetAttack();
		if (stageName == U"trapBoss" && isTrapBossSecondPhaseStarted && !isTrapBossSecondPhaseDefeated && !isTrapBossSecondPhaseDefeatedFall) {
			trapBossSecondPhaseHpBarDelay.update(trapBossSecondPhaseHp, trapBossSecondPhaseMaxHp);
		}
		// カメラ位置 + 揺れ
		camera.setTargetCenter(executeCameraPos() + cameraShake.getOffset());
		camera.update(); {
			const auto t = camera.createTransformer();

			player->update();

			// 弾丸の生成
			if (player->getIsGenerateBullet()) {
				if (bullets.size() < bulletMaxNum) {
					bullets << std::make_shared<Bullet>(player->pos, player->getDirection() == Global::Direction::RIGHT ? bulletSpeed : -bulletSpeed, player.get());
					Sound::playOneShot(Sound::SHOOT);
				}
				player->setIsGenerateBullet(false);
			}

			// 血しぶきの生成
			if (player->getIsDead() && !isGenerateBloods) {
				double circleNum = 2;
				double deltaD = 360 / bloodNum;
				for (int32 count = 0; count < circleNum; count++) {
					for (int32 i = 0; i < bloodNum / circleNum; i++) {
						bloods << std::make_shared<Blood>(player->pos, i * deltaD);
					}
				}
				isGenerateBloods = true;
			}

			for (auto& b : bloods) {
				stockNearGameObjects.add(b.get());
				b->update();
			}

			//毎フレームGameObjectをspatialGridに登録
			stockNearGameObjects.clear();
			stockBulletsNearGameObjects.clear();
			stockLargeNearGameObjects.clear();

			stockNearGameObjects.add(player.get());
			stockLargeNearGameObjects.add(player.get());

			// 対ブロック
			for (auto& b : blocks) {
				b->update();
				stockNearGameObjects.add(b.get());
				stockBulletsNearGameObjects.add(b.get());
				if (b->isTriggerTrap) {
					const bool shouldBreakByBossDefeat = (stageName == U"trapBoss")
						? isTrapBossSecondPhaseDefeated
						: Global::isBossDefeated;
					b->trapUpdate(shouldBreakByBossDefeat ? 0 : -1);
				}
			}

			// 針の更新と、起動しているトリガーIDの反映
			for (auto& s : spikes) {
				s->update();
				stockNearGameObjects.add(s.get());
			}

			for (auto& b : bullets) {
				b->update();
			}
			//通常の弾幕用りんご
			for (auto& c : cherries) {
				c->update();
				stockNearGameObjects.add(c.get());
			}
			//ボスりんご
			Vec2 bossCherryPos;
			BossCherryType attackCherryType;
			for (auto& bc : bossCherries) {
				bc->update();

				if (auto* b = dynamic_cast<BossCherry*>(bc.get())) {
					bossCherryPos = b->pos;
					defeatedBossNum = b->getDefeatedBossNum();
					attackCherryType = b->getBossCherryAttackType();
				}
				else if (auto* bs = dynamic_cast<BossSubCherry*>(bc.get())) {
					bs->setCenterPos(bossCherryPos);
					bs->setDefeatedBossNum(defeatedBossNum);
					bs->generateAttack(attackCherryType);
				}

				stockNearGameObjects.add(bc.get());
				stockBulletsNearGameObjects.add(bc.get());
			}
			// 一時格納したりんごをここでまとめて追加
			for (auto& c : pendingCherries) {
				cherries << c;
			}
			pendingCherries.clear();


			//セーブ関連
			for (auto& s : savePoints) {
				s->update();
				stockBulletsNearGameObjects.add(s.get());
			}
			for (auto& w : warps) {
				stockNearGameObjects.add(w.get());
			}

			//playerの近くのオブジェクトのみを取得して当たり判定確認
			auto near = stockNearGameObjects.query(player->getBroadRect());
			for (auto* obj : near) {
				if (obj == player.get()) continue;
				player->onCollision(*obj);
			}
			//血のブロックに対する衝突
			if (!bloods.isEmpty()) {
				for (auto& b : bloods) {
					auto nearObjs = stockNearGameObjects.query(b->getBroadRect());
					for (auto* obj : nearObjs) {
						b->onCollision(*obj);
					}
				}
			}
			//トリガーなど広範囲で衝突を確認する必要のあるオブジェクトに当たり判定確認
			near = stockLargeNearGameObjects.query(player->getBroadRect());
			for (auto* obj : near) {
				if (obj == player.get()) continue;
				player->onCollision(*obj);
			}

			player->updateLate();

			//各弾丸とブロック,セーブポイントとの衝突
			for (auto& b : bullets) {
				auto nearObjs = stockBulletsNearGameObjects.query(b->getBroadRect());
				for (auto* obj : nearObjs) {
					b->onCollision(*obj);
				}
			}
			updateTrapBossSecondPhaseBulletHits(bullets);

			// 倒されたボスりんごのサブりんご取得用処理
			if (!bossCherries.empty()) {
				auto* bossCherry = dynamic_cast<BossCherry*>(bossCherries.front().get());
				if (bossCherry) {
					for (auto& bc : bossCherries) {
						auto* bs = dynamic_cast<BossSubCherry*>(bc.get());
						if (bs && bs->isDelete) {
							bossCherry->removeDefeatedAttackType(bs->getBossCherrySubType());
						}
					}
				}
			}

			if (stageName == U"trapBoss" && isTrapBossSecondPhaseStarted && !isTrapBossSecondPhaseDefeated) {
				//暗転演出の透明度を変更
				if (trapBossSecondPhaseDarkAlpha > trapBossSecondPhaseDarkAlphaMax) {
					trapBossSecondPhaseDarkAlpha -= trapBossSecondPhaseDarkAlphaFadeSpeed;
				}
				else {
					if (darkAlphaTimer.reachedZero()) {
						trapBossSecondPhaseDarkAlpha = Random(trapBossSecondPhaseDarkAlphaMin, trapBossSecondPhaseDarkAlphaMax);
						darkAlphaTimer.restart();
					}
				}
			}

			// ----- 以下削除処理 -----

			//画面外のりんごを削除
			cherries.remove_if([](auto&& cherry) {
				return cherry->isOutOfScreen || cherry->isDelete;
			});
			//ボスりんごを削除
			bossCherries.remove_if([](auto&& cherry) {
				return cherry->isOutOfScreen || cherry->isDelete;
			});

			//画面外の針を削除
			spikes.remove_if([](auto&& spike) {
				return spike->isOutOfScreen;
			});

			//画面外の血を削除
			bloods.remove_if([](auto&& blood) {
				return blood->isOutOfScreen;
			});

			//ブロック削除
			blocks.remove_if([](auto&& block) {
				return block->isDelete;
			});

			//セーブ削除
			savePoints.remove_if([](auto&& save) {
				return save->isDelete;
			});

			//弾丸削除
			bullets.remove_if([](auto&& bullet) {
				return bullet->isOutOfScreen || bullet->isDelete;
			});
		}
	}

	void BossStageManager::updateTrapBossSecondPhaseIntro() {
		if (stageName != U"trapBoss" || !Global::isBossDefeated || isTrapBossSecondPhaseStarted) {
			return;
		}

		if (!isTrapBossSecondPhaseIntroStarted) {
			isTrapBossSecondPhaseIntroStarted = true;
			trapBossSecondPhaseIntroStopwatch.restart();
		}

		if (trapBossSecondPhaseIntroStopwatch.sF() >= trapBossSecondPhaseIntroCooldown + trapBossSecondPhaseIntroTime) {
			isTrapBossSecondPhaseStarted = true;
			trapBossSecondPhaseTayamaCenterPos = Vec2{ 370, 304 };
			trapBossSecondPhaseTayamaAngle = 0.0;
			trapBossSecondPhaseDefeatedFallSpeed = 0.0;
			backgroundName = U"background_trapBossCave2";
			breakTrapBossSecondPhaseOverlappingBlocks();
			cameraShake.shake(trapBossSecondPhaseStartShakeTime, trapBossSecondPhaseStartShakePower);
			bossBgmStart = true;
			trapBossSecondPhaseEyeAttackCount = 0;
			trapBossSecondPhaseTargetAttackCount = 0;
			trapBossSecondPhaseAttackStopwatch.restart();
			trapBossSecondPhaseTargetAttackStopwatch.restart();
			Sound::playOneShot(Sound::VC_HAKKYOU);
		}
	}

	void BossStageManager::updateTrapBossSecondPhaseDefeatedFall() {
		if (stageName != U"trapBoss" || !isTrapBossSecondPhaseDefeatedFall) {
			return;
		}

		trapBossSecondPhaseDefeatedFallSpeed += trapBossSecondPhaseDefeatedFallAcceleration;
		trapBossSecondPhaseTayamaCenterPos.y += trapBossSecondPhaseDefeatedFallSpeed;
		trapBossSecondPhaseTayamaAngle += trapBossSecondPhaseDefeatedRotateSpeed * Scene::DeltaTime();

		if (trapBossSecondPhaseTayamaCenterPos.y > Global::stageHeight + TextureAsset(U"tayama").height() * trapBossSecondPhaseTayamaScale) {
			isTrapBossSecondPhaseDefeatedFall = false;
		}
	}

	void BossStageManager::updateTrapBossSecondPhaseEyeAttack() {
		if (stageName != U"trapBoss" || !isTrapBossSecondPhaseStarted || isTrapBossSecondPhaseDefeated || isTrapBossSecondPhaseDefeatedFall) {
			return;
		}

		const double nextAttackTime = trapBossSecondPhaseEyeAttackStartDelay
			+ trapBossSecondPhaseEyeAttackCount * trapBossSecondPhaseEyeAttackInterval;
		if (trapBossSecondPhaseAttackStopwatch.sF() < nextAttackTime) {
			return;
		}

		const double angleOffset = trapBossSecondPhaseEyeAttackCount * trapBossSecondPhaseEyeAttackAngleStep;
		createTrapBossSecondPhaseEyeAttackCherry(
			getTrapBossSecondPhaseLeftEyePos(),
			trapBossSecondPhaseEyeAttackBaseDirection + angleOffset);
		createTrapBossSecondPhaseEyeAttackCherry(
			getTrapBossSecondPhaseRightEyePos(),
			trapBossSecondPhaseEyeAttackBaseDirection - angleOffset);

		++trapBossSecondPhaseEyeAttackCount;
	}

	void BossStageManager::updateTrapBossSecondPhaseTargetAttack() {
		if (stageName != U"trapBoss" || !isTrapBossSecondPhaseStarted || isTrapBossSecondPhaseDefeated || isTrapBossSecondPhaseDefeatedFall) {
			return;
		}

		const double nextAttackTime = trapBossSecondPhaseTargetAttackStartDelay
			+ trapBossSecondPhaseTargetAttackCount * trapBossSecondPhaseTargetAttackInterval;
		if (trapBossSecondPhaseTargetAttackStopwatch.sF() < nextAttackTime) {
			return;
		}

		createTrapBossSecondPhaseTargetAttackCherry(getTrapBossSecondPhaseLeftEyePos());
		createTrapBossSecondPhaseTargetAttackCherry(getTrapBossSecondPhaseRightEyePos());
		++trapBossSecondPhaseTargetAttackCount;
	}

	void BossStageManager::breakTrapBossSecondPhaseOverlappingBlocks() {
		if (hasTrapBossSecondPhaseBrokenBlocks) {
			return;
		}

		hasTrapBossSecondPhaseBrokenBlocks = true;

		const double breakWidth = (trapBossSecondPhaseBreakBlockRange * 2 + 1) * oneTileSize;
		const Vec2 centerBlockTopLeft{
			Floor(Global::windowWidth / 2.0 / oneTileSize) * oneTileSize,
			Floor(Global::windowHeight / 2.0 / oneTileSize) * oneTileSize
		};
		const RectF breakArea{
			Vec2{ centerBlockTopLeft.x - trapBossSecondPhaseBreakBlockRange * oneTileSize, 0 },
			SizeF{ breakWidth, static_cast<double>(Global::stageHeight) }
		};

		for (auto& block : gameObjects.blocks) {
			if (block->getIsDebris()
				|| !block->getBroadRect().intersects(breakArea)) {
				continue;
			}

			block->breakAsDebris();
		}
	}

	void BossStageManager::createTrapBossSecondPhaseEyeAttackCherry(Vec2 startPos, double direction) {
		auto cherry = std::make_shared<TayamaSecondPhaseEyeCherry>(
			startPos,
			trapBossSecondPhaseEyeAttackCherryScale,
			trapBossSecondPhaseEyeAttackCherrySpeed,
			trapBossSecondPhaseEyeAttackCherryAcceleration);
		cherry->direction = direction;
		cherry->setCustomTexture(U"sprCherryTrap", 32, true);
		createCherry(cherry);
	}

	void BossStageManager::createTrapBossSecondPhaseTargetAttackCherry(Vec2 startPos) {
		auto cherry = std::make_shared<TayamaSecondPhaseTargetCherry>(
			startPos,
			trapBossSecondPhaseTargetAttackCherryScale,
			trapBossSecondPhaseTargetAttackCherrySpeed,
			trapBossSecondPhaseTargetAttackMoveTime,
			trapBossSecondPhaseTargetAttackStopTime,
			trapBossSecondPhaseTargetAttackMoveCount,
			*this);
		cherry->setCustomTexture(U"sprCherryTrap", 32, true);
		cherry->setCustomTextureColor(ColorF{ 1.0, 0.15, 0.15 });
		createCherry(cherry);
	}

	void BossStageManager::updateTrapBossSecondPhaseBulletHits(Array<std::shared_ptr<Bullet>>& bullets) {
		if (stageName != U"trapBoss" || !isTrapBossSecondPhaseStarted || isTrapBossSecondPhaseDefeated || isTrapBossSecondPhaseDefeatedFall) {
			return;
		}

		const Circle leftEye{ getTrapBossSecondPhaseLeftEyePos(), trapBossSecondPhaseEyeHitRadius };
		const Circle rightEye{ getTrapBossSecondPhaseRightEyePos(), trapBossSecondPhaseEyeHitRadius };

		for (auto& bullet : bullets) {
			if (bullet->isDelete || bullet->isOutOfScreen) {
				continue;
			}

			if (const auto bulletCircle = bullet->hitBox->getCircle()) {
				if (bulletCircle->intersects(leftEye) || bulletCircle->intersects(rightEye)) {
					bullet->isDelete = true;
					hitTrapBossSecondPhase();
				}
			}
		}
	}

	void BossStageManager::debug() {
		auto& player = gameObjects.player;

		if (Global::inputDebugMuteki.down()) {
			player->setIsMuteki(!player->getIsMuteki());
		}

		ClearPrint();
		Print << U" Stage Step : " << step;
		Print << U" Player Pos : " << player->pos;
		Print << U" Player Muteki : " << player->getIsMuteki();
		Print << U" Cherries Num : " << gameObjects.cherries.size();
	}

	void BossStageManager::draw() {
		//背景描画
		drawTrapBossSecondPhaseIntro();
		TextureAsset(backgroundName).draw();

		Array<GameObject*> drawList;
		drawList.reserve(
			gameObjects.spikes.size()
			+ gameObjects.blocks.size()
			+ gameObjects.savePoints.size()
			+ gameObjects.warps.size()
			+ gameObjects.cherries.size()
			+ gameObjects.bossCherries.size()
			+ gameObjects.bloods.size()
			+ gameObjects.bullets.size()
			+ 1);

		// 所有権のコピーを避け、描画中だけ有効なポインタを格納する
		for (const auto& s : gameObjects.spikes) drawList << s.get();
		for (const auto& b : gameObjects.blocks) drawList << b.get();
		for (const auto& s : gameObjects.savePoints) drawList << s.get();
		for (const auto& w : gameObjects.warps) drawList << w.get();
		for (const auto& c : gameObjects.cherries) drawList << c.get();
		for (const auto& c : gameObjects.bossCherries) drawList << c.get();
		for (const auto& b : gameObjects.bloods) drawList << b.get();
		for (const auto& b : gameObjects.bullets) drawList << b.get();
		drawList << gameObjects.player.get();

		// ソート
		drawList.sort_by([](const auto& a, const auto& b) {
			return a->depth < b->depth;
		});

		{
			const auto t = camera.createTransformer();

			if (Global::nowRoomName == U"ExBoss") {
				TextureAsset(backgroundName).draw();
			}

			drawTrapBossSecondPhaseTayama();
			drawTrapBossSecondPhaseEyeHitBoxes();

			// 描画
			for (auto& obj : drawList) obj->draw();

			if (stageName == U"trapBoss" && isTrapBossSecondPhaseStarted && !isTrapBossSecondPhaseDefeated) {
				//暗転演出
				Rect(0, 0, 1600, 608).draw(ColorF(0.0, 0.0, 0.0, trapBossSecondPhaseDarkAlpha));
			}

			//GAMEOVER描画
			if (isShowGameOver) {
				const StringView gameOverTextureName = (Global::mainTextureNumber == 0) ? U"sprGAMEOVER_low" : U"sprGAMEOVER_normal";
				TextureAsset(gameOverTextureName).drawAt(executeCameraPos());
			}
		}

		titleCard.draw();

		if (Global::nowRoomName == U"ExBoss") {
			Rect(0, 544, 800, 64).draw(ColorF(Palette::Black));
		}

		if (Global::getItem1) {
			int32 nowPlayerHp = gameObjects.player->getHp();
			for (int i = 0; i < nowPlayerHp; i++) {
				TextureAsset(U"heart").draw(playerHpBasePos.x + i * hpInterbalX, playerHpBasePos.y);
			}
		}

		drawTrapBossSecondPhaseHp();
	}

	void BossStageManager::drawTrapBossSecondPhaseIntro() const {
		if (!isTrapBossSecondPhaseIntroStarted || isTrapBossSecondPhaseStarted) {
			return;
		}

		const double introElapsed = trapBossSecondPhaseIntroStopwatch.sF() - trapBossSecondPhaseIntroCooldown;
		if (introElapsed < 0.0) {
			return;
		}

		const double t = Min(1.0, introElapsed / trapBossSecondPhaseIntroTime);
		const Vec2 drawPos = trapBossSecondPhaseTayamaStartPos.lerp(trapBossSecondPhaseTayamaEndPos, EaseOutQuad(t));

		TextureAsset(U"tayama")
			.scaled(trapBossSecondPhaseTayamaScale)
			.drawAt(drawPos);
	}

	void BossStageManager::drawTrapBossSecondPhaseTayama() const {
		if (stageName != U"trapBoss" || !isTrapBossSecondPhaseStarted || (isTrapBossSecondPhaseDefeated && !isTrapBossSecondPhaseDefeatedFall)) {
			return;
		}

		TextureAsset(U"tayama")
			.scaled(trapBossSecondPhaseTayamaScale)
			.rotated(Math::ToRadians(trapBossSecondPhaseTayamaAngle))
			.drawAt(trapBossSecondPhaseTayamaCenterPos);
	}

	void BossStageManager::drawTrapBossSecondPhaseEyeHitBoxes() const {
		if (stageName != U"trapBoss" || !isTrapBossSecondPhaseStarted || isTrapBossSecondPhaseDefeated
			|| isTrapBossSecondPhaseDefeatedFall
			|| !isTrapBossSecondPhaseEyeHitFlash
			|| trapBossSecondPhaseEyeHitFlashStopwatch.sF() >= trapBossSecondPhaseEyeHitFlashTime) {
			return;
		}

		const double alphaRate = 1.0 - trapBossSecondPhaseEyeHitFlashStopwatch.sF() / trapBossSecondPhaseEyeHitFlashTime;
		const ColorF hitColor{ 1.0, 0.0, 0.0, trapBossSecondPhaseEyeHitFlashAlpha * alphaRate };
		const ColorF frameColor{ 1.0, 1.0, 1.0, trapBossSecondPhaseEyeHitFlashAlpha * alphaRate };

		for (const auto& eyePos : getTrapBossSecondPhaseEyePositions()) {
			Circle{ eyePos, trapBossSecondPhaseEyeHitRadius }.draw(hitColor);
			Circle{ eyePos, trapBossSecondPhaseEyeHitRadius }.drawFrame(2.0, frameColor);
		}
	}

	void BossStageManager::drawTrapBossSecondPhaseHp() const {
		if (stageName != U"trapBoss" || !isTrapBossSecondPhaseStarted || isTrapBossSecondPhaseDefeated || isTrapBossSecondPhaseDefeatedFall) {
			return;
		}

		const double width = Global::stageWidth;
		const double height = 20.0;
		const Vec2 barPos{ Global::stageWidth / 2.0, 0.0 };
		drawBossHpBar(barPos, width, height, trapBossSecondPhaseHp, trapBossSecondPhaseMaxHp, 1.0, trapBossSecondPhaseHpBarDelay);

		const String bossName = U"Boss : Tayama";
		const Vec2 textPos{ 6, 18 };
		for (const Vec2 offset : { Vec2{-1, 0}, Vec2{1, 0}, Vec2{0, -1}, Vec2{0, 1} }) {
			FontAsset(U"BossHp")(bossName).draw(textPos + offset, ColorF{ 0.0, 0.0, 0.0 });
		}
		FontAsset(U"BossHp")(bossName).draw(textPos, ColorF{ 1.0, 1.0, 1.0 });
	}

	void BossStageManager::hitTrapBossSecondPhase() {
		if (isTrapBossSecondPhaseDefeated || isTrapBossSecondPhaseDefeatedFall || trapBossSecondPhaseHp <= 0) {
			return;
		}

		Sound::playOneShot(Sound::BOSSHIT);
		--trapBossSecondPhaseHp;
		isTrapBossSecondPhaseEyeHitFlash = true;
		trapBossSecondPhaseEyeHitFlashStopwatch.restart();

		if (trapBossSecondPhaseHp <= 0) {
			Sound::playOneShot(Sound::DEATH);
			defeatTrapBossSecondPhase();
		}
	}

	void BossStageManager::defeatTrapBossSecondPhase() {
		if (isTrapBossSecondPhaseDefeated || isTrapBossSecondPhaseDefeatedFall) {
			return;
		}

		isTrapBossSecondPhaseDefeated = true;
		clearTrapBossSecondPhaseCherries();
		Sound::playOneShot(Sound::VC_BAKANA);
		if (Global::moraleValue2 >= 90 && Global::moraleValue3 >= 90 && Global::moraleValue4 >= 90) {
			Global::endingValue = 6;
		}
		isTrapBossSecondPhaseDefeatedFall = true;
		trapBossSecondPhaseDefeatedFallSpeed = 0.0;
	}

	void BossStageManager::clearTrapBossSecondPhaseCherries() {
		gameObjects.cherries.clear();
		pendingCherries.clear();
	}

	void BossStageManager::setStep(int32 newStep) {
		step = newStep;
	}

	// セーブ処理
	void BossStageManager::saveGame() {
		//プレイヤーの位置を保存
		Global::savedStartPlayerPos = gameObjects.player->pos;
		Global::savedRoomName = stageName;
		Global::isExistSaveData = true;
	}

	// カメラの位置をプレイヤーのいるエリアの中心に設定
	Vec2 BossStageManager::executeCameraPos() {
		Vec2 nextPos;

		if (Global::isCameraFollowMode) {
			nextPos.x = static_cast<int32>(gameObjects.player->pos.x);
			nextPos.y = Global::stageHeight / 2;

			if (nextPos.x < Global::windowWidth / 2) nextPos.x = Global::windowWidth / 2;
			else if (nextPos.x > Global::stageWidth - Global::windowWidth / 2) nextPos.x = Global::stageWidth - Global::windowWidth / 2;

			return nextPos;
		}

		int32 playerAreaX = static_cast<int32>(gameObjects.player->pos.x) / Global::windowWidth;
		int32 playerAreaY = static_cast<int32>(gameObjects.player->pos.y) / Global::windowHeight;
		nextPos.x = playerAreaX * Global::windowWidth + Global::windowWidth / 2;
		nextPos.y = playerAreaY * Global::windowHeight + Global::windowHeight / 2;
		return nextPos;
	}

	void BossStageManager::generateBoss(int32 type) {

		switch (type) {
		case 1://boss召喚
			gameObjects.bossCherries <<  std::make_shared<BossCherry>(Vec2{ getPlayer()->pos.x,getPlayer()->pos.y + 500}, 5.0, *this);
			gameObjects.bossCherries <<  std::make_shared<BossSubCherry>(Vec2{ 100, -100}, 2.0, BossCherryType::Red, *this);
			gameObjects.bossCherries <<  std::make_shared<BossSubCherry>(Vec2{ 200, -100}, 2.0, BossCherryType::Blue, *this);
			gameObjects.bossCherries <<  std::make_shared<BossSubCherry>(Vec2{ 300, -100}, 2.0, BossCherryType::Yellow, *this);
			gameObjects.bossCherries <<  std::make_shared<BossSubCherry>(Vec2{ 400, -100}, 2.0, BossCherryType::Green, *this);
			gameObjects.bossCherries <<  std::make_shared<BossSubCherry>(Vec2{ 500, -100}, 2.0, BossCherryType::Orange, *this);
			gameObjects.bossCherries <<  std::make_shared<BossSubCherry>(Vec2{ 600, -100}, 2.0, BossCherryType::Sky, *this);
			bossBgmStart = true;
			darkAlpha = 0.9;
			cameraShake.shake(0.4, 20.0);
			titleCard.startShowTitleCard(U"boss");
			break;
		case 2://Exボス召喚
			gameObjects.bossCherries << std::make_shared<ExBossCherry>(Vec2{ 800,-300 }, 5.0, *this);
			bossBgmStart = true;
			darkAlpha = 0.9;
			cameraShake.shake(0.4, 20.0);
			titleCard.startShowTitleCard(U"ExBoss");
			break;
		case 3://罠ボス召喚
			gameObjects.bossCherries << std::make_shared<TayamaBoss>(
				Vec2{ Global::stageWidth / 2.0, Global::stageHeight + 140.0 }, 1.5, *this);
			bossBgmStart = true;
			darkAlpha = 0.9;
			cameraShake.shake(0.4, 20.0);
			break;
		case 4://簡易ボス召喚
			gameObjects.bossCherries << std::make_shared<LowBossCherry>(
				Vec2{ Global::stageWidth - 120, Global::stageHeight + 80.0 }, 5.0, *this);
			bossBgmStart = true;
			darkAlpha = 0.9;
			cameraShake.shake(0.4, 20.0);
			//titleCard.startShowTitleCard(U"boss");
			break;
		}
	}

	std::shared_ptr<Player> BossStageManager::getPlayer() {
		return gameObjects.player;
	}

	Array<std::shared_ptr<Cherry>> BossStageManager::getCherries() {
		return gameObjects.cherries;
	}

	Array<std::shared_ptr<Block>> BossStageManager::getBlocks() {
		return gameObjects.blocks;
	}

	std::shared_ptr<SordCherriesManager> BossStageManager::getExBossSordManagerCherry() {
		if (stageName == U"ExBoss") {
			return std::dynamic_pointer_cast<SordCherriesManager>(gameObjects.bossCherries.front());
		}
		return nullptr;
	}

	String BossStageManager::getStageName() const {
		return stageName;
	}

	bool BossStageManager::shouldStopBossBgm() const {
		if (stageName == U"trapBoss" && isTrapBossSecondPhaseStarted && !isTrapBossSecondPhaseDefeated) {
			return false;
		}

		return Global::isBossDefeated;
	}

	bool BossStageManager::isTrapBossSecondPhaseBgm() const {
		return stageName == U"trapBoss" && isTrapBossSecondPhaseStarted;
	}

	Vec2 BossStageManager::getTrapBossSecondPhaseLeftEyePos() const {
		return trapBossSecondPhaseTayamaCenterPos + trapBossSecondPhaseLeftEyeOffset * trapBossSecondPhaseTayamaScale;
	}

	Vec2 BossStageManager::getTrapBossSecondPhaseRightEyePos() const {
		return trapBossSecondPhaseTayamaCenterPos + trapBossSecondPhaseRightEyeOffset * trapBossSecondPhaseTayamaScale;
	}

	Array<Vec2> BossStageManager::getTrapBossSecondPhaseEyePositions() const {
		return{ getTrapBossSecondPhaseLeftEyePos(), getTrapBossSecondPhaseRightEyePos() };
	}

	//りんご生成と管理配列への追加
	void BossStageManager::createCherry(std::shared_ptr<Cherry> cherry) {
		if (stageName == U"trapBoss" && trapBossGuygunStopwatch.sF() >= trapBossGuygunInterval) {
			Sound::playOneShot(Sound::GUYGUN, trapBossGuygunVolume);
			trapBossGuygunStopwatch.restart();
		}

		pendingCherries << cherry;
	}

	//外周のブロック配置
	void BossStageManager::createPeripheryBlocks() {
		gameObjects.blocks << std::make_shared<Block>(U"sprBlock", Vec2(0, 0));
		gameObjects.blocks << std::make_shared<Block>(U"sprBlock", Vec2(24, 0));
		gameObjects.blocks << std::make_shared<Block>(U"sprBlock", Vec2(0, 18));
		gameObjects.blocks << std::make_shared<Block>(U"sprBlock", Vec2(24, 18));
		for (int i = 1; i < 18; i++) {
			gameObjects.blocks << std::make_shared<Block>(U"sprWall", Vec2(0, i));
			gameObjects.blocks << std::make_shared<Block>(U"sprWall", Vec2(24, i));
		}
		for (int i = 1; i < 24; i++) {
			gameObjects.blocks << std::make_shared<Block>(U"sprFloor", Vec2(i, 0));
			gameObjects.blocks << std::make_shared<Block>(U"sprFloor", Vec2(i, 18));
		}
	}

	//5マス分の床ブロックを作成
	void BossStageManager::createFloorBlocks(Vec2 basePos) {
		gameObjects.blocks << std::make_shared<Block>(U"sprBlock", Vec2(basePos.x, basePos.y));
		for (int i = 1; i <= 3; i++) {
			gameObjects.blocks << std::make_shared<Block>(U"sprFloor", Vec2(basePos.x + i, basePos.y));
		}
		gameObjects.blocks << std::make_shared<Block>(U"sprBlock", Vec2(basePos.x + 4, basePos.y));
	}
}
