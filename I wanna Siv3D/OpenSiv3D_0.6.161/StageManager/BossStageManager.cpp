#include "BossStageManager.h"
#include "../Audio/AudioAsset.h"
#include "../MainGameSerializer.h"

namespace Iwanna {
	namespace {
		String formatCooldownSeconds(double seconds) {
			const double displaySeconds = Ceil(Max(0.0, seconds) * 10.0) / 10.0;
			const int32 whole = static_cast<int32>(displaySeconds);
			const int32 decimal = static_cast<int32>(Round((displaySeconds - whole) * 10.0));
			return Format(whole) + U"." + Format(decimal);
		}
	}

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
		pendingBossCherries.clear();
		gameObjects.blocks.clear();
		gameObjects.spikes.clear();
		gameObjects.savePoints.clear();
		gameObjects.bloods.clear();
		gameObjects.warps.clear();
		gameObjects.items.clear();
		gameObjects.signs.clear();

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
		isExBossDarkEffectActive = false;
		exBossDarkAlpha = 0.0;
		exBossEntryDarkAlpha = 0.0;
		isExBossThirdPhaseDarkening = false;
		isExBossThirdPhaseRestoring = false;
		isExBossSaveActivated = false;
		isExBossDefeatEffectStarted = false;
		exBossDefeatEffectStopwatch.reset();
		exBossDefeatWhiteFlashAlpha = 0.0;
		exBossDefeatFinalWhiteFadeAlpha = 0.0;
		isExBossCameraLocked = false;
		hasExBossThirdPhaseLowBoss = false;
		hasExBossThirdPhaseBossCherry = false;
		hasExBossThirdPhaseTayama = false;
		isExBossThirdPhaseTayamaLeaving = false;
		exBossThirdPhaseTayamaStep = 0;
		exBossThirdPhaseTayamaLeaveVelocity = 0.0;
		isExBossSummonNameBarWaiting = false;
		isExBossSummonNameBarActive = false;
		trapBossSecondPhaseIntroStopwatch.reset();
		trapBossSecondPhaseEyeHitFlashStopwatch.reset();
		trapBossSecondPhaseAttackStopwatch.reset();
		trapBossSecondPhaseTargetAttackStopwatch.reset();
		shouldCleanupBossCherryDefeatObjects = false;

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
				if (stageName == U"ExBoss") {
					isExBossSaveActivated = true;
				}
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
		const String blockQuarity = (fileName == U"ExBoss") ? U"normal" : quarity;

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
				case 1: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_" + blockQuarity + U"1", pos); break;
				case 2: gameObjects.blocks << std::make_shared<VisualOnlyBlock>(U"sprBlock_extra4", pos); break;
				case 6: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_" + blockQuarity + U"2", pos); break;
				case 7: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_" + blockQuarity + U"3", pos); break;
				case 20: gameObjects.blocks << std::make_shared<HaibokusyaBlock>(pos); break;
				case 16: if (Global::shouldShowMorale2Spike()) gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 0); break;
				case 17: if (Global::shouldShowMorale2Spike()) gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 1); break;
				case 18: if (Global::shouldShowMorale2Spike()) gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 2); break;
				case 19: if (Global::shouldShowMorale2Spike()) gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 3); break;
				case 21: gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 0); break;
				case 22: gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 1); break;
				case 23: gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 2); break;
				case 24: gameObjects.spikes << std::make_shared<Spike>(quarity, pos, 3); break;
				//case 25: gameObjects.savePoints << std::make_shared<SavePoint>(pos); break;
				case 26: gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_" + blockQuarity + U"1", pos); break;
				case 27: gameObjects.blocks << std::make_shared<ShootTroughBlock>(U"sprBlockShootTrough", pos); break;
				case 28: gameObjects.blocks << std::make_shared<FakeBlock>(U"sprBlock_" + blockQuarity + U"2", pos); break;
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

					const String breakBlockTextureName = (fileName == U"ExBoss") ? U"sprBlock_normal1" : U"sprBlock_" + blockQuarity + U"3";
					if (gimmikName == U"罠ブロック") {
						auto block = std::make_shared<BreakBlock>(breakBlockTextureName, gimmikParsePos, static_cast<int32>(gimmikValue1));
						block->setPlayBreakSound(fileName != U"ExBoss");
						gameObjects.blocks << block;
					}
					if (gimmikName == U"時間罠ブロック") {
						auto block = std::make_shared<TimedBreakBlock>(breakBlockTextureName, gimmikParsePos, static_cast<int32>(gimmikValue1), gimmikValue2);
						block->setPlayBreakSound(fileName != U"ExBoss");
						gameObjects.blocks << block;
					}
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
			gameObjects.player->setHp(3);
			gameObjects.savePoints << std::make_shared<BossSavePoint>(Vec2{400,500},1);
		}
		if (stageName == U"bossLow") {
			gameObjects.player->setHp(1);
			gameObjects.savePoints << std::make_shared<BossSavePoint>(Vec2{ 400,500 }, 4);
		}
		if (stageName == U"ExBoss") {
			gameObjects.player->setHp(3);
			gameObjects.savePoints << std::make_shared<BossSavePoint>(Vec2{ 800,450 }, 2);
			gameObjects.signs << std::make_shared<Sign>(Vec2{ 11,15 }, SignType::Extra_Warp);
			gameObjects.bossCherries << std::make_shared<SordCherriesManager>(Vec2{ 800,430 }, 2.0, *this);
			Global::isCameraFollowMode = true;
			Global::isBossExBarrageAttack = true;
			exBossEntryDarkAlpha = 1.0;

			if (Global::isExistSaveData && !Global::isChangeRoom && Global::savedRoomName == U"ExBoss") {
				gameObjects.savePoints.clear();
				Global::doNotStopBgm = true;
				isExBossSaveActivated = true;
				generateBoss(2);
			}
			if (Global::isEndingKRoute()) {
				saveGame();
			}
		}
		if (stageName == U"trapBoss") {
			gameObjects.player->setHp(3);
			gameObjects.savePoints << std::make_shared<BossSavePoint>(Vec2{ 400,500 }, 3);
		}
	}

	Vec2 BossStageManager::parsePos(const JSON& json) {
		return Vec2{ json[0].get<double>() / oneTileSize, json[1].get<double>() / oneTileSize };
	}

	Vec2 BossStageManager::parseIntactPos(const JSON& json) {
		return Vec2{ json[0].get<double>(), json[1].get<double>() };
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
		auto& items = gameObjects.items;
		auto& signs = gameObjects.signs;

		//死亡判定
		if (player->getIsDead()) {
			gameoverTimer.start();
			if (gameoverTimer.reachedZero()) {
				isShowGameOver = true;
			}
		}

		//タイトルカード処理
		titleCard.update();
		achive.update();

		// 揺れ更新
		cameraShake.update();
		updateTrapBossSecondPhaseIntro();
		updateTrapBossSecondPhaseDefeatedFall();
		updateTrapBossSecondPhaseEyeAttack();
		updateTrapBossSecondPhaseTargetAttack();
		updateExBossThirdPhaseTayamaSummon();
		updateExBossSummonNameBar();
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

			if (player->getIsGenerateWarpEffect()) {
				createPlayerWarpEffectCherries(player->getWarpEffectPos());
				player->setIsGenerateWarpEffect(false);
			}

			// 血しぶきの生成
			if (player->getIsDead() && !isGenerateBloods && Global::canShowDeathBloodEffect()) {
				double circleNum = 2;
				double deltaD = 360 / bloodNum;
				for (int32 count = 0; count < circleNum; count++) {
					for (int32 i = 0; i < bloodNum / circleNum; i++) {
						bloods << std::make_shared<Blood>(player->pos, i * deltaD);
					}
				}
				isGenerateBloods = true;
			}
			else if (player->getIsDead() && !Global::canShowDeathBloodEffect()) {
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
					const bool shouldBreakByBossDefeat = (stageName == U"ExBoss")
						? isExBossSaveActivated
						: (stageName == U"trapBoss")
						? isTrapBossSecondPhaseDefeated
						: (stageName == U"boss")
						? Global::isBossDefeated && Global::canUseItem2Effect()
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
			for (auto& sign : signs) {
				sign->update();
				stockNearGameObjects.add(sign.get());
			}
			//通常の弾幕用りんご
			for (auto& c : cherries) {
				c->update();
				stockNearGameObjects.add(c.get());
			}
			//ボスりんご
			Vec2 bossCherryPos{ 0, 0 };
			bool hasBossCherryPos = false;
			BossCherryType attackCherryType = BossCherryType::None;
			for (auto& bc : bossCherries) {
				bc->update();

				if (auto* b = dynamic_cast<BossCherry*>(bc.get())) {
					bossCherryPos = b->pos;
					hasBossCherryPos = true;
					defeatedBossNum = b->getDefeatedBossNum();
					attackCherryType = b->getBossCherryAttackType();
				}
				else if (auto* bs = dynamic_cast<BossSubCherry*>(bc.get())) {
					if (hasBossCherryPos) {
						bs->setCenterPos(bossCherryPos);
						bs->setDefeatedBossNum(defeatedBossNum);
						bs->generateAttack(attackCherryType);
					}
				}

				stockNearGameObjects.add(bc.get());
				stockBulletsNearGameObjects.add(bc.get());
			}
			// 一時格納したりんごをここでまとめて追加
			for (auto& c : pendingCherries) {
				cherries << c;
			}
			pendingCherries.clear();
			for (auto& c : pendingBossCherries) {
				bossCherries << c;
			}
			pendingBossCherries.clear();


			//セーブ関連
			for (auto& s : savePoints) {
				s->update();
				stockBulletsNearGameObjects.add(s.get());
			}
			for (auto& w : warps) {
				stockNearGameObjects.add(w.get());
			}
			for (auto& i : items) {
				i->update();
				stockNearGameObjects.add(i.get());
			}

			//playerの近くのオブジェクトのみを取得して当たり判定確認
			auto near = stockNearGameObjects.query(player->getBroadRect());
			for (auto* obj : near) {
				if (obj == player.get() || obj->type != ObjectType::Block) continue;
				player->onCollision(*obj);
			}
			near = stockNearGameObjects.query(player->getBroadRect());
			for (auto* obj : near) {
				if (obj == player.get() || obj->type == ObjectType::Block) continue;
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
			updateItem2Pickup();

			player->updateLate();

			//item1取得後の弾丸と針の衝突
			updateBulletSpikeHits();

			//各弾丸とブロック,セーブポイントとの衝突
			for (auto& b : bullets) {
				auto nearObjs = stockBulletsNearGameObjects.query(b->getBroadRect());
				for (auto* obj : nearObjs) {
					b->onCollision(*obj);
				}
			}
			updateTrapBossSecondPhaseBulletHits(bullets);

			if (shouldCleanupBossCherryDefeatObjects) {
				cleanupBossCherryDefeatObjects();
			}

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

			if (stageName == U"ExBoss" && isExBossDarkEffectActive) {
				if (isExBossThirdPhaseRestoring) {
					exBossDarkAlpha = Max(0.0, exBossDarkAlpha - exBossThirdPhaseRestoreSpeed);
					if (exBossDarkAlpha <= 0.0) {
						isExBossThirdPhaseRestoring = false;
						isExBossDarkEffectActive = false;
					}
				}
				else if (isExBossThirdPhaseDarkening) {
					exBossDarkAlpha = Min(exBossThirdPhaseDarkAlphaTarget, exBossDarkAlpha + exBossThirdPhaseDarkAlphaSpeed);
				}
				else if (exBossDarkAlpha > exBossDarkAlphaMax) {
					exBossDarkAlpha = Max(exBossDarkAlphaMax, exBossDarkAlpha - exBossDarkAlphaFadeSpeed);
				}
				else if (darkAlphaTimer.reachedZero()) {
					exBossDarkAlpha = exBossDarkAlphaMin + Random(exBossDarkAlphaMax - exBossDarkAlphaMin);
					darkAlphaTimer.restart();
				}
			}

			if (stageName == U"ExBoss" && exBossEntryDarkAlpha > 0.0) {
				exBossEntryDarkAlpha = Max(0.0, exBossEntryDarkAlpha - exBossEntryDarkAlphaFadeSpeed);
			}

			if (stageName == U"ExBoss" && exBossDefeatWhiteFlashAlpha > 0.0) {
				exBossDefeatWhiteFlashAlpha = Max(0.0, exBossDefeatWhiteFlashAlpha - exBossDefeatWhiteFlashFadeSpeed);
			}

			if (stageName == U"ExBoss" && isExBossDefeatEffectStarted) {
				const double elapsed = exBossDefeatEffectStopwatch.sF();
				if (elapsed >= exBossDefeatFinalWhiteFadeStartTime) {
					const double fadeElapsed = elapsed - exBossDefeatFinalWhiteFadeStartTime;
					exBossDefeatFinalWhiteFadeAlpha = Min(1.0, fadeElapsed / exBossDefeatFinalWhiteFadeDuration);
					if (exBossDefeatFinalWhiteFadeAlpha >= 1.0) {
						System::Exit();
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
				return spike->isOutOfScreen || spike->isDelete;
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

			//アイテム削除
			items.remove_if([](auto&& item) {
				return item->isDelete;
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

	void BossStageManager::updateExBossThirdPhaseTayamaSummon() {
		if (stageName != U"ExBoss" || !hasExBossThirdPhaseTayama) {
			return;
		}

		switch (exBossThirdPhaseTayamaStep) {
		case 0:
		{
			const double t = Min(1.0, exBossThirdPhaseTayamaStopwatch.sF() / exBossThirdPhaseTayamaAppearDuration);
			exBossThirdPhaseTayamaCenterPos = exBossThirdPhaseTayamaStartPos.lerp(exBossThirdPhaseTayamaTargetPos, EaseOutQuad(t));
			if (t >= 1.0) {
				exBossThirdPhaseTayamaCenterPos = exBossThirdPhaseTayamaTargetPos;
				exBossThirdPhaseTayamaStopwatch.restart();
				exBossThirdPhaseTayamaAttackStopwatch.restart();
				exBossThirdPhaseTayamaTargetAttackStopwatch.restart();
				exBossThirdPhaseTayamaEyeAttackCount = 0;
				exBossThirdPhaseTayamaTargetAttackCount = 0;
				exBossThirdPhaseTayamaStep = 1;
			}
			break;
		}
		case 1:
		{
			const double nextEyeAttackTime = trapBossSecondPhaseEyeAttackStartDelay
				+ exBossThirdPhaseTayamaEyeAttackCount * trapBossSecondPhaseEyeAttackInterval;
			if (exBossThirdPhaseTayamaAttackStopwatch.sF() >= nextEyeAttackTime) {
				const double angleOffset = exBossThirdPhaseTayamaEyeAttackCount * trapBossSecondPhaseEyeAttackAngleStep;
				createTrapBossSecondPhaseEyeAttackCherry(
					exBossThirdPhaseTayamaCenterPos + trapBossSecondPhaseLeftEyeOffset * exBossThirdPhaseTayamaScale,
					trapBossSecondPhaseEyeAttackBaseDirection + angleOffset);
				createTrapBossSecondPhaseEyeAttackCherry(
					exBossThirdPhaseTayamaCenterPos + trapBossSecondPhaseRightEyeOffset * exBossThirdPhaseTayamaScale,
					trapBossSecondPhaseEyeAttackBaseDirection - angleOffset);
				++exBossThirdPhaseTayamaEyeAttackCount;
			}

			const double nextTargetAttackTime = trapBossSecondPhaseTargetAttackStartDelay
				+ exBossThirdPhaseTayamaTargetAttackCount * trapBossSecondPhaseTargetAttackInterval;
			if (exBossThirdPhaseTayamaTargetAttackStopwatch.sF() >= nextTargetAttackTime) {
				createTrapBossSecondPhaseTargetAttackCherry(
					exBossThirdPhaseTayamaCenterPos + trapBossSecondPhaseLeftEyeOffset * exBossThirdPhaseTayamaScale);
				createTrapBossSecondPhaseTargetAttackCherry(
					exBossThirdPhaseTayamaCenterPos + trapBossSecondPhaseRightEyeOffset * exBossThirdPhaseTayamaScale);
				++exBossThirdPhaseTayamaTargetAttackCount;
			}

			if (exBossThirdPhaseTayamaStopwatch.sF() >= exBossThirdPhaseTayamaLifeTime) {
				isExBossThirdPhaseTayamaLeaving = true;
				exBossThirdPhaseTayamaLeaveVelocity = -exBossThirdPhaseTayamaLeaveSpeed;
				exBossThirdPhaseTayamaStep = 2;
			}
			break;
		}
		case 2:
			exBossThirdPhaseTayamaLeaveVelocity -= exBossThirdPhaseTayamaLeaveAcceleration;
			exBossThirdPhaseTayamaCenterPos.y += exBossThirdPhaseTayamaLeaveVelocity;
			if (exBossThirdPhaseTayamaCenterPos.y < exBossLockedCameraCenter.y - Global::windowHeight / 2.0 - TextureAsset(U"tayama").height() * exBossThirdPhaseTayamaScale) {
				hasExBossThirdPhaseTayama = false;
				isExBossThirdPhaseTayamaLeaving = false;
			}
			break;
		}
	}

	void BossStageManager::updateExBossSummonNameBar() {
		if (stageName != U"ExBoss") {
			isExBossSummonNameBarWaiting = false;
			isExBossSummonNameBarActive = false;
			return;
		}

		if (isExBossSummonNameBarWaiting && exBossSummonNameBarWaitingStopwatch.sF() >= exBossSummonNameBarWaitingDelay) {
			startExBossSummonNameBar(exBossSummonNameBarWaitingTextureName, exBossSummonNameBarWaitingCenterPos);
			isExBossSummonNameBarWaiting = false;
		}

		const double totalTime = exBossSummonNameBarFadeInTime + exBossSummonNameBarShowTime + exBossSummonNameBarFadeOutTime;
		if (isExBossSummonNameBarActive && exBossSummonNameBarStopwatch.sF() >= totalTime) {
			isExBossSummonNameBarActive = false;
		}
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

		bool hasBrokenBlock = false;
		for (auto& block : gameObjects.blocks) {
			if (block->getIsDebris()
				|| !block->getBroadRect().intersects(breakArea)) {
				continue;
			}

			block->breakAsDebris(false);
			hasBrokenBlock = true;
		}

		if (hasBrokenBlock) {
			Sound::playOneShot(Sound::BLOCKBREAK);
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
					hitTrapBossSecondPhase(Global::canUseItem1Effect() ? 2 : 1);
				}
			}
		}
	}

	void BossStageManager::updateBulletSpikeHits() {
		if (!Global::canUseItem1Effect()) {
			return;
		}

		for (auto& bullet : gameObjects.bullets) {
			if (bullet->isDelete || bullet->isOutOfScreen) {
				continue;
			}

			for (auto& spike : gameObjects.spikes) {
				if (spike->isDelete || spike->isOutOfScreen || spike->getIsDebris()) {
					continue;
				}

				bullet->onCollision(*spike);
				if (bullet->isDelete) {
					break;
				}
			}
		}
	}

	void BossStageManager::updateItem2Pickup() {
		if (Global::getItem2) {
			return;
		}

		for (auto& item : gameObjects.items) {
			if (item->getItemType() == ItemType::Warp && item->isPlayerTouching) {
				Global::getItem2 = true;
				achive.startShowAchieve(AchieveType::ItemGet_Warp);
				MainGameSerializer serializer;
				serializer.SaveGameSave();
				saveGame();
				break;
			}
		}
	}

	void BossStageManager::debug() {
		auto& player = gameObjects.player;

		if (stageName == U"ExBoss") {
			return;
		}

		if (Global::canUseHaibokusyaMode() && Global::inputDebugMuteki.down()) {
			player->toggleIsMuteki();
			Global::recordHaibokusyaModeUseIfNeeded();
		}

		/*
		ClearPrint();
		Print << U" Stage Step : " << step;
		Print << U" Player Pos : " << player->pos;
		Print << U" Player Muteki : " << player->getIsMuteki();
		Print << U" Cherries Num : " << gameObjects.cherries.size();
		*/
	}

	void BossStageManager::draw() {
		//背景描画
		drawTrapBossSecondPhaseIntro();
		if (stageName != U"ExBoss") {
			TextureAsset(backgroundName).draw();
		}

		Array<GameObject*> drawList;
		drawList.reserve(
			gameObjects.spikes.size()
			+ gameObjects.blocks.size()
			+ gameObjects.savePoints.size()
			+ gameObjects.warps.size()
			+ gameObjects.items.size()
			+ gameObjects.cherries.size()
			+ gameObjects.bossCherries.size()
			+ gameObjects.bloods.size()
			+ gameObjects.bullets.size()
			+ gameObjects.signs.size()
			+ 1);

		// 所有権のコピーを避け、描画中だけ有効なポインタを格納する
		for (const auto& s : gameObjects.spikes) drawList << s.get();
		for (const auto& b : gameObjects.blocks) drawList << b.get();
		for (const auto& s : gameObjects.savePoints) drawList << s.get();
		for (const auto& w : gameObjects.warps) drawList << w.get();
		for (const auto& i : gameObjects.items) drawList << i.get();
		for (const auto& s : gameObjects.signs) drawList << s.get();
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
				const double backgroundBrightness = Clamp(1.0 - exBossDarkAlpha, 0.0, 1.0);
				TextureAsset(backgroundName).draw(ColorF(backgroundBrightness, backgroundBrightness, backgroundBrightness));
			}

			drawTrapBossSecondPhaseTayama();
			drawExBossThirdPhaseTayama();
			drawTrapBossSecondPhaseEyeHitBoxes();

			// 描画
			for (auto& obj : drawList) obj->draw();
			drawExBossSummonNameBar();

			if (stageName == U"trapBoss" && isTrapBossSecondPhaseStarted && !isTrapBossSecondPhaseDefeated) {
				//暗転演出
				Rect(0, 0, 1600, 608).draw(ColorF(0.0, 0.0, 0.0, trapBossSecondPhaseDarkAlpha));
			}

			if (stageName == U"ExBoss" && exBossEntryDarkAlpha > 0.0) {
				Rect(0, 0, Global::stageWidth, Global::stageHeight).draw(ColorF(0.0, 0.0, 0.0, exBossEntryDarkAlpha));
			}

			//GAMEOVER描画
			if (isShowGameOver) {
				const StringView gameOverTextureName = (Global::mainTextureNumber == 0) ? U"sprGAMEOVER_low" : U"sprGAMEOVER_normal";
				TextureAsset(gameOverTextureName).drawAt(executeCameraPos());
			}
		}

		titleCard.draw();

		if (stageName == U"ExBoss" && !isExBossDefeatEffectStarted) {
			for (const auto& cherry : gameObjects.bossCherries) {
				if (const auto* exBoss = dynamic_cast<const ExBossCherry*>(cherry.get())) {
					exBoss->drawHpBarScreen();
				}
			}
		}

		if (Global::nowRoomName == U"ExBoss") {
			Rect(0, 544, 800, 64).draw(ColorF(Palette::Black));
		}

		if (stageName == U"boss" || stageName == U"trapBoss" || stageName == U"ExBoss") {
			int32 nowPlayerHp = gameObjects.player->getHp();
			for (int i = 0; i < nowPlayerHp; i++) {
				TextureAsset(U"heart").draw(playerHpBasePos.x + i * hpInterbalX, playerHpBasePos.y);
			}
		}

		if (stageName == U"ExBoss" && Global::canUseExBossItem2Effect()) {
			const Vec2 warpIconPos{ playerHpBasePos.x + hpInterbalX * 3 + 20, playerHpBasePos.y };
			const double cooldownRemaining = gameObjects.player->getExBossWarpCooldownRemaining();
			const bool canWarp = gameObjects.player->canUseExBossItem2Warp();
			const ColorF iconColor{ 1.0, canWarp ? 1.0 : 0.35 };

			TextureAsset(U"item2").draw(warpIconPos, iconColor);
			if (cooldownRemaining > 0.0) {
				const String cooldownText = formatCooldownSeconds(cooldownRemaining);
				FontAsset(U"Button")(cooldownText).drawAt(warpIconPos + Vec2{ 17, 17 }, ColorF{ 0.0, 0.0, 0.0, 0.8 });
				FontAsset(U"Button")(cooldownText).drawAt(warpIconPos + Vec2{ 16, 16 }, ColorF{ 1.0, 1.0, 1.0 });
			}
		}

		drawTrapBossSecondPhaseHp();
		if (stageName == U"ExBoss" && exBossDefeatWhiteFlashAlpha > 0.0) {
			Rect{ 0, 0, Global::windowWidth, Global::windowHeight }.draw(ColorF{ 1.0, 1.0, 1.0, exBossDefeatWhiteFlashAlpha });
		}
		if (stageName == U"ExBoss" && exBossDefeatFinalWhiteFadeAlpha > 0.0) {
			Rect{ 0, 0, Global::windowWidth, Global::windowHeight }.draw(ColorF{ 1.0, 1.0, 1.0, exBossDefeatFinalWhiteFadeAlpha });
		}
		achive.draw();
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

	void BossStageManager::drawExBossThirdPhaseTayama() const {
		if (stageName != U"ExBoss" || !hasExBossThirdPhaseTayama) {
			return;
		}

		TextureAsset(U"tayama")
			.scaled(exBossThirdPhaseTayamaScale)
			.drawAt(exBossThirdPhaseTayamaCenterPos);
	}

	void BossStageManager::drawExBossSummonNameBar() const {
		if (stageName != U"ExBoss" || !isExBossSummonNameBarActive || exBossSummonNameBarTextureName == U"") {
			return;
		}

		const double elapsed = exBossSummonNameBarStopwatch.sF();
		double alpha = 1.0;
		if (elapsed < exBossSummonNameBarFadeInTime) {
			alpha = elapsed / exBossSummonNameBarFadeInTime;
		}
		else if (elapsed > exBossSummonNameBarFadeInTime + exBossSummonNameBarShowTime) {
			const double fadeOutElapsed = elapsed - exBossSummonNameBarFadeInTime - exBossSummonNameBarShowTime;
			alpha = 1.0 - fadeOutElapsed / exBossSummonNameBarFadeOutTime;
		}

		alpha = Clamp(alpha, 0.0, 1.0);
		TextureAsset(exBossSummonNameBarTextureName).draw(exBossSummonNameBarCenterPos - exBossSummonNameBarAnchor, ColorF{ 1.0, alpha });
	}

	void BossStageManager::drawTrapBossSecondPhaseHp() const {
		if (stageName != U"trapBoss" || !isTrapBossSecondPhaseStarted || isTrapBossSecondPhaseDefeated || isTrapBossSecondPhaseDefeatedFall) {
			return;
		}

		const double width = Global::stageWidth;
		const double height = 20.0;
		const Vec2 barPos{ Global::stageWidth / 2.0, 0.0 };
		drawBossHpBar(barPos, width, height, trapBossSecondPhaseHp, trapBossSecondPhaseMaxHp, 1.0, trapBossSecondPhaseHpBarDelay);

		const String bossName = U"Boss : ????";
		const Vec2 textPos{ 6, 18 };
		for (const Vec2 offset : { Vec2{-1, 0}, Vec2{1, 0}, Vec2{0, -1}, Vec2{0, 1} }) {
			FontAsset(U"BossHp")(bossName).draw(textPos + offset, ColorF{ 0.0, 0.0, 0.0 });
		}
		FontAsset(U"BossHp")(bossName).draw(textPos, ColorF{ 1.0, 1.0, 1.0 });
	}

	void BossStageManager::hitTrapBossSecondPhase(int32 damage) {
		if (isTrapBossSecondPhaseDefeated || isTrapBossSecondPhaseDefeatedFall || trapBossSecondPhaseHp <= 0) {
			return;
		}

		Sound::playOneShot(Sound::BOSSHIT);
		trapBossSecondPhaseHp -= Max(1, damage);
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
		isTrapBossSecondPhaseDefeatedFall = true;
		trapBossSecondPhaseDefeatedFallSpeed = 0.0;
	}

	void BossStageManager::clearTrapBossSecondPhaseCherries() {
		gameObjects.cherries.clear();
		pendingCherries.clear();
	}

	void BossStageManager::requestBossCherryDefeatCleanup() {
		shouldCleanupBossCherryDefeatObjects = true;
	}

	void BossStageManager::cleanupBossCherryDefeatObjects() {
		gameObjects.cherries.clear();
		pendingCherries.clear();
		gameObjects.bossCherries.remove_if([](const std::shared_ptr<Cherry>& cherry) {
			return dynamic_cast<BossSubCherry*>(cherry.get()) != nullptr;
		});
		if (stageName == U"boss" && !Global::getItem2 && gameObjects.items.isEmpty()) {
			spawnItem2();
		}
		shouldCleanupBossCherryDefeatObjects = false;
	}

	void BossStageManager::spawnItem2() {
		const Vec2 itemPixelPos{ 384, 544 };
		gameObjects.items << std::make_shared<Item>(itemPixelPos / oneTileSize, ItemType::Warp);
	}

	void BossStageManager::setStep(int32 newStep) {
		step = newStep;
	}

	// セーブ処理
	void BossStageManager::saveGame() {
		//プレイヤーの位置を保存
		Global::savedStartPlayerPos = gameObjects.player->pos;
		Global::savedRoomName = stageName;
		Global::savedIsWarpMode = Global::canUseItem2Effect() && gameObjects.player->getIsWarpMode();
		Global::isExistSaveData = true;
		MainGameSerializer serializer;
		serializer.SaveExtraProgress();
	}

	// カメラの位置をプレイヤーのいるエリアの中心に設定
	Vec2 BossStageManager::executeCameraPos() {
		Vec2 nextPos;
		const double halfWindowWidth = Global::windowWidth / 2.0;
		const double halfWindowHeight = Global::windowHeight / 2.0;

		if (isExBossCameraLocked) {
			return exBossLockedCameraCenter;
		}

		if (Global::isCameraFollowMode) {
			nextPos.x = Clamp(gameObjects.player->pos.x, halfWindowWidth, Max(halfWindowWidth, Global::stageWidth - halfWindowWidth));
			nextPos.y = Global::stageHeight / 2;

			return nextPos;
		}

		const double maxPlayerX = Max(0.0, Global::stageWidth - 1.0);
		const double maxPlayerY = Max(0.0, Global::stageHeight - 1.0);
		const Vec2 clampedPlayerPos{
			Clamp(gameObjects.player->pos.x, 0.0, maxPlayerX),
			Clamp(gameObjects.player->pos.y, 0.0, maxPlayerY)
		};

		const int32 playerAreaX = static_cast<int32>(clampedPlayerPos.x) / Global::windowWidth;
		const int32 playerAreaY = static_cast<int32>(clampedPlayerPos.y) / Global::windowHeight;
		nextPos.x = playerAreaX * Global::windowWidth + halfWindowWidth;
		nextPos.y = playerAreaY * Global::windowHeight + halfWindowHeight;
		nextPos.x = Clamp(nextPos.x, halfWindowWidth, Max(halfWindowWidth, Global::stageWidth - halfWindowWidth));
		nextPos.y = Clamp(nextPos.y, halfWindowHeight, Max(halfWindowHeight, Global::stageHeight - halfWindowHeight));
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
			gameObjects.signs.clear();
			gameObjects.bossCherries << std::make_shared<ExBossCherry>(Vec2{ 800,-300 }, 5.0, *this);
			bossBgmStart = true;
			Global::doNotStopBgm = true;
			darkAlpha = 0.9;
			isExBossDarkEffectActive = true;
			exBossDarkAlpha = 1.0;
			darkAlphaTimer.restart();
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

	void BossStageManager::startExBossDefeatEffect() {
		if (stageName != U"ExBoss" || isExBossDefeatEffectStarted) {
			return;
		}

		isExBossDefeatEffectStarted = true;
		exBossDefeatEffectStopwatch.restart();
		exBossDefeatWhiteFlashAlpha = exBossDefeatWhiteFlashStartAlpha;
		exBossDefeatFinalWhiteFadeAlpha = 0.0;
		Global::doNotStopBgm = false;
		Global::isBossDefeated = true;
		gameObjects.cherries.clear();
		pendingCherries.clear();
		pendingBossCherries.clear();
		gameObjects.bossCherries.remove_if([](const std::shared_ptr<Cherry>& cherry) {
			return !dynamic_cast<ExBossCherry*>(cherry.get())
				&& !dynamic_cast<SordCherriesManager*>(cherry.get());
		});
	}

	bool BossStageManager::getIsExBossDefeatEffectStarted() const {
		return isExBossDefeatEffectStarted;
	}

	bool BossStageManager::isTrapBossSecondPhaseBgm() const {
		return stageName == U"trapBoss" && isTrapBossSecondPhaseStarted;
	}

	void BossStageManager::startExBossThirdPhaseDarkening() {
		if (stageName != U"ExBoss") {
			return;
		}

		isExBossDarkEffectActive = true;
		isExBossThirdPhaseDarkening = true;
		isExBossThirdPhaseRestoring = false;
		exBossDarkAlpha = Max(exBossDarkAlpha, exBossDarkAlphaMax);
	}

	bool BossStageManager::isExBossThirdPhaseDarkened() const {
		return stageName == U"ExBoss" && isExBossThirdPhaseDarkening && exBossDarkAlpha >= exBossThirdPhaseDarkAlphaTarget;
	}

	void BossStageManager::summonExBossThirdPhaseLowBoss() {
		if (stageName != U"ExBoss" || hasExBossThirdPhaseLowBoss) {
			return;
		}

		exBossLockedCameraCenter = executeCameraPos();
		isExBossCameraLocked = true;
		Global::isCameraFollowMode = false;

		const double startY = exBossLockedCameraCenter.y - Global::windowHeight / 2.0 - 96.0;
		auto lowBoss = std::make_shared<LowBossCherry>(
			Vec2{ exBossLockedCameraCenter.x, startY },
			exBossLowBossScale,
			*this);
		lowBoss->setAppearanceSettings(exBossLowBossTargetY, exBossLowBossAppearDuration, true);
		lowBoss->setLifeTime(exBossLowBossLifeTime);
		lowBoss->setHpBarVisible(false);
		lowBoss->setSpreadAttackSettings(exBossLowBossSpreadInterval, exBossLowBossSpreadCherryNum, exBossLowBossSpreadCherrySpeed);
		lowBoss->setTargetAttackSettings(
			exBossLowBossTargetInterval,
			exBossLowBossTargetLineNum,
			exBossLowBossTargetIsAddLine,
			exBossLowBossTargetBaseSpeed,
			exBossLowBossTargetIntervalSpeed);
		pendingBossCherries << lowBoss;
		hasExBossThirdPhaseLowBoss = true;
		reserveExBossSummonNameBar(U"sprNameBar_low", Vec2{ exBossLockedCameraCenter.x, exBossLowBossTargetY }, exBossLowBossAppearDuration);
	}

	bool BossStageManager::isExBossThirdPhaseLowBossFinished() const {
		if (stageName != U"ExBoss" || !hasExBossThirdPhaseLowBoss) {
			return false;
		}

		for (const auto& cherry : gameObjects.bossCherries) {
			if (dynamic_cast<const LowBossCherry*>(cherry.get())) {
				return false;
			}
		}

		for (const auto& cherry : pendingBossCherries) {
			if (dynamic_cast<const LowBossCherry*>(cherry.get())) {
				return false;
			}
		}

		return true;
	}

	void BossStageManager::summonExBossThirdPhaseBossCherry() {
		if (stageName != U"ExBoss" || hasExBossThirdPhaseBossCherry) {
			return;
		}

		exBossLockedCameraCenter = executeCameraPos();
		isExBossCameraLocked = true;
		Global::isCameraFollowMode = false;

		const double startY = exBossLockedCameraCenter.y - Global::windowHeight / 2.0 - 96.0;
		auto bossCherry = std::make_shared<BossCherry>(
			Vec2{ exBossLockedCameraCenter.x, startY },
			exBossBossCherryScale,
			*this);
		bossCherry->setSummonPatternSettings(
			exBossBossCherryTargetY,
			exBossBossCherryAppearDuration,
			exBossBossCherryLifeTime,
			exBossBossCherryAttackInterval);
		bossCherry->setHpBarVisible(false);
		pendingBossCherries << bossCherry;

		const Array<BossCherryType> funnelTypes{
			BossCherryType::Red,
			BossCherryType::Blue,
			BossCherryType::Yellow,
			BossCherryType::Green,
			BossCherryType::Orange,
			BossCherryType::Sky
		};
		for (const auto type : funnelTypes) {
			auto funnel = std::make_shared<BossSubCherry>(
				Vec2{ exBossLockedCameraCenter.x, startY },
				exBossBossCherryFunnelScale,
				type,
				*this);
			funnel->setSummonPattern(true);
			pendingBossCherries << funnel;
		}

		hasExBossThirdPhaseBossCherry = true;
		reserveExBossSummonNameBar(U"sprNameBar_normal", Vec2{ exBossLockedCameraCenter.x, exBossBossCherryTargetY }, exBossBossCherryAppearDuration);
	}

	bool BossStageManager::isExBossThirdPhaseBossCherryFinished() {
		if (stageName != U"ExBoss" || !hasExBossThirdPhaseBossCherry) {
			return false;
		}

		for (const auto& cherry : gameObjects.bossCherries) {
			if (auto* bossCherry = dynamic_cast<BossCherry*>(cherry.get())) {
				if (bossCherry->getIsSummonPattern()) {
					return false;
				}
			}
		}

		for (const auto& cherry : pendingBossCherries) {
			if (auto* bossCherry = dynamic_cast<BossCherry*>(cherry.get())) {
				if (bossCherry->getIsSummonPattern()) {
					return false;
				}
			}
		}

		for (const auto& cherry : gameObjects.bossCherries) {
			auto* subCherry = dynamic_cast<BossSubCherry*>(cherry.get());
			if (subCherry != nullptr && subCherry->getIsSummonPattern()) {
				cherry->isDelete = true;
			}
		}
		for (const auto& cherry : pendingBossCherries) {
			auto* subCherry = dynamic_cast<BossSubCherry*>(cherry.get());
			if (subCherry != nullptr && subCherry->getIsSummonPattern()) {
				cherry->isDelete = true;
			}
		}
		return true;
	}

	void BossStageManager::summonExBossThirdPhaseTayama() {
		if (stageName != U"ExBoss" || hasExBossThirdPhaseTayama) {
			return;
		}

		exBossLockedCameraCenter = executeCameraPos();
		isExBossCameraLocked = true;
		Global::isCameraFollowMode = false;

		exBossThirdPhaseTayamaStartPos = Vec2{
			exBossLockedCameraCenter.x,
			exBossLockedCameraCenter.y - Global::windowHeight / 2.0 - TextureAsset(U"tayama").height() * exBossThirdPhaseTayamaScale
		};
		exBossThirdPhaseTayamaTargetPos = Vec2{ exBossLockedCameraCenter.x, exBossThirdPhaseTayamaTargetY };
		exBossThirdPhaseTayamaCenterPos = exBossThirdPhaseTayamaStartPos;
		exBossThirdPhaseTayamaStep = 0;
		exBossThirdPhaseTayamaStopwatch.restart();
		exBossThirdPhaseTayamaAttackStopwatch.reset();
		exBossThirdPhaseTayamaTargetAttackStopwatch.reset();
		exBossThirdPhaseTayamaEyeAttackCount = 0;
		exBossThirdPhaseTayamaTargetAttackCount = 0;
		exBossThirdPhaseTayamaLeaveVelocity = 0.0;
		isExBossThirdPhaseTayamaLeaving = false;
		hasExBossThirdPhaseTayama = true;
		reserveExBossSummonNameBar(U"sprNameBar_trap", exBossThirdPhaseTayamaTargetPos, exBossThirdPhaseTayamaAppearDuration);
		Sound::playOneShot(Sound::VC_BAAN);
	}

	bool BossStageManager::isExBossThirdPhaseTayamaFinished() const {
		if (stageName != U"ExBoss") {
			return false;
		}

		return !hasExBossThirdPhaseTayama;
	}

	void BossStageManager::reserveExBossSummonNameBar(String textureName, Vec2 centerPos, double delay) {
		isExBossSummonNameBarWaiting = true;
		exBossSummonNameBarWaitingTextureName = textureName;
		exBossSummonNameBarWaitingCenterPos = centerPos;
		exBossSummonNameBarWaitingDelay = Max(0.0, delay);
		exBossSummonNameBarWaitingStopwatch.restart();
	}

	void BossStageManager::startExBossSummonNameBar(String textureName, Vec2 centerPos) {
		exBossSummonNameBarTextureName = textureName;
		exBossSummonNameBarCenterPos = centerPos;
		isExBossSummonNameBarActive = true;
		exBossSummonNameBarStopwatch.restart();
	}

	void BossStageManager::finishExBossThirdPhaseLowBoss() {
		if (stageName != U"ExBoss") {
			return;
		}

		isExBossCameraLocked = false;
		Global::isCameraFollowMode = true;
		isExBossThirdPhaseDarkening = false;
		isExBossThirdPhaseRestoring = true;
		isExBossDarkEffectActive = true;
	}

	void BossStageManager::startExBossForthFormLongAttackSetup() {
		if (stageName != U"ExBoss") {
			return;
		}

		exBossLockedCameraCenter = executeCameraPos();
		isExBossCameraLocked = true;
		Global::isCameraFollowMode = false;
		startExBossThirdPhaseDarkening();
	}

	Vec2 BossStageManager::getExBossLockedCameraCenter() const {
		return exBossLockedCameraCenter;
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
		const bool shouldPlayTrapBossGuygun = stageName == U"trapBoss"
			|| (stageName == U"ExBoss" && hasExBossThirdPhaseTayama && exBossThirdPhaseTayamaStep == 1);
		if (shouldPlayTrapBossGuygun && trapBossGuygunStopwatch.sF() >= trapBossGuygunInterval) {
			Sound::playOneShot(Sound::GUYGUN, trapBossGuygunVolume);
			trapBossGuygunStopwatch.restart();
		}

		pendingCherries << cherry;
	}

	void BossStageManager::createPlayerWarpEffectCherries(Vec2 centerPos) {
		const int32 cherryNum = 18;
		const double cherrySpeed = 4.0;
		const double deltaDirection = 360.0 / cherryNum;

		for (int32 i = 0; i < cherryNum; ++i) {
			gameObjects.cherries << std::make_shared<WarpEffectAppleCherry>(
				centerPos,
				i * deltaDirection,
				cherrySpeed);
		}
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
