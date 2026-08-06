#include "StageManager.h"
#include "../Audio/AudioAsset.h"
#include "../MainGameSerializer.h"

namespace Iwanna {
	namespace {
		String formatPlayTime(double seconds) {
			const int32 totalSeconds = static_cast<int32>(seconds);
			const int32 minutes = totalSeconds / 60;
			const int32 displaySeconds = totalSeconds % 60;
			const int32 centiseconds = static_cast<int32>((seconds - totalSeconds) * 100);

			const String secondText = (displaySeconds < 10 ? U"0" : U"") + Format(displaySeconds);
			const String centisecondText = (centiseconds < 10 ? U"0" : U"") + Format(centiseconds);
			return Format(minutes) + U":" + secondText + U"." + centisecondText;
		}

		ColorF getExtraStageMainColor(const String& stageName) {
			if (stageName == U"ExMiluArea") return ColorF{ Palette::White };
			if (stageName == U"ExMochiArea") return ColorF{ Palette::Orange };
			if (stageName == U"ExGotArea") return ColorF{ Palette::Gray };
			if (stageName == U"ExRyutaArea") return ColorF{ 0.32, 0.62, 1.0 };
			if (stageName == U"ExRenoArea") return ColorF{ Palette::Yellow };
			return ColorF{ Palette::White };
		}

		void drawExtraStageBackground(const String& stageName) {
			Rect{ 0, 0, Global::windowWidth, Global::windowHeight }.draw(ColorF{ 0.0, 0.0, 0.0 });

			const ColorF mainColor = getExtraStageMainColor(stageName);
			const ColorF lineColor{ mainColor.r, mainColor.g, mainColor.b, 0.32 };
			const double spacing = 54.0;
			const double speed = 26.0;
			const double offset = Fmod(Scene::Time() * speed, spacing);
			const Vec2 scroll{ -offset, offset };
			const double extent = Global::windowWidth + Global::windowHeight + spacing * 4.0;

			for (double x = -extent - offset; x < extent; x += spacing) {
				Line{ Vec2{ x, -spacing } + scroll, Vec2{ x + extent, extent - spacing } + scroll }.draw(2.0, lineColor);
				Line{ Vec2{ x, Global::windowHeight + spacing } + scroll, Vec2{ x + extent, Global::windowHeight - extent + spacing } + scroll }.draw(2.0, lineColor);
			}
		}
	}

	StageManager::StageManager() {
		stockNearGameObjects.cellSize = 96;
		stockBulletsNearGameObjects.cellSize = 96;
		stockLargeNearGameObjects.cellSize = 800;
	}

	void StageManager::setUpObjects(int32 chapter) {
		gameObjects.player = std::make_shared<Player>();

		// 既存のオブジェクトを抹消して初期化
		gameObjects.bullets.clear();
		gameObjects.cherries.clear();
		gameObjects.blocks.clear();
		gameObjects.spikes.clear();
		gameObjects.triggers.clear();
		gameObjects.savePoints.clear();
		gameObjects.specialTraps.clear();
		gameObjects.specialBackTraps.clear();
		gameObjects.bloods.clear();
		gameObjects.warps.clear();
		gameObjects.signs.clear();
		gameObjects.items.clear();

		// 一部変数の初期化
		isGenerateBloods = false;
		playedTrapPonTriggerIDs.clear();
		hasSecretEntranceFlashShown = false;
		secretEntranceFlashAlpha = 0.0;

		Global::trap2MapBgmStop = false;
		Global::trapActivatedId30InTrap2Map = false;
		Global::trapCameraActivatedInTrap2Map = false;
		Global::isPlayerFrozen = false;
		Global::isSecretTriggerActivated = false;

		gameoverTimer.reset();
		isShowGameOver = false;

		titleCard.reset();

		if (Global::isChangeRoom) {
			loadGameObjects(Global::nowRoomName);

			camera.jumpTo(executeCameraPos(), 1.0);

			//隠しアイテムマップ時のみタイトルカード表示
			if(Global::nowRoomName == U"secret1") titleCard.startShowTitleCard(U"secret1");

			if (Global::prepareGetItem1
				&& Global::prevRoomName == U"secret1"
				&& Global::nowRoomName == U"normal7") {
				achive.startShowAchieve(AchieveType::ItemGet_Heart);
				Global::prepareGetItem1 = false;
				Global::getItem1 = true;
				MainGameSerializer serializer;
				serializer.SaveCharactersMoraleValue();
				saveGame();
			}
		}
		else {
			loadGameObjects(Global::savedRoomName);
		}

		if (Global::isExtraStage(stageName)) {
			titleCard.setEntryByPlayerPosition(gameObjects.player->pos, executeCameraPos());
			titleCard.startShowTitleCard(stageName);
		}

		if (stageName == U"clear") {
			saveGame();
			if (Global::isNoMoraleEndingRoute()) {
				Global::endingValue = 9;
			}
			else if (Global::isEndingKRoute()) {
				Global::endingValue = 10;
			}
			else if (Global::isEndingDRoute()) {
				Global::endingValue = 3;
			}
			else if (Global::deathCount == 0) {
				Global::endingValue = 8;
			}
			else if (Global::isEndingGRoute()) {
				Global::endingValue = 6;
			}
			else if (Global::isEndingHRoute()) {
				Global::endingValue = 7;
			}
			else if (Global::isEndingBRoute()) {
				Global::endingValue = 1;
			}
			else if (Global::isEndingCRoute()) {
				Global::endingValue = 2;
			}
			else {
				Global::endingValue = 0;
			}
		}

		Global::isChangeRoom = false;

		// セーブ時コールバックは、生成直後に一度だけ設定する
		for (auto& savePoint : gameObjects.savePoints) {
			savePoint->onSavedCallback = [this]() {
				saveGame();
			};
		}

		if (!(Global::prevRoomName == U"secret1" && Global::nowRoomName == U"normal7")) {
			Global::prepareGetItem1 = false;
		}
	}

	Vec2 StageManager::parsePos(const JSON& json) {
		return Vec2{ json[0].get<double>() / oneTileSize, json[1].get<double>() / oneTileSize };
	}

	Vec2 StageManager::parseIntactPos(const JSON& json) {
		return Vec2{ json[0].get<double>(), json[1].get<double>() };
	}

	void StageManager::update() {

		// ----- update関連 -----
		auto& player = gameObjects.player;
		auto& bullets = gameObjects.bullets;
		auto& cherries = gameObjects.cherries;
		auto& blocks = gameObjects.blocks;
		auto& spikes = gameObjects.spikes;
		auto& triggers = gameObjects.triggers;
		auto& savePoints = gameObjects.savePoints;
		auto& specialTraps = gameObjects.specialTraps;
		auto& specialBackTraps = gameObjects.specialBackTraps;
		auto& bloods = gameObjects.bloods;
		auto& warps = gameObjects.warps;
		auto& signs = gameObjects.signs;
		auto& items = gameObjects.items;

		if (player->getIsDead()) {
			gameoverTimer.start();
			if (gameoverTimer.reachedZero()) {
				isShowGameOver = true;
			}
		}

		//trap2 mapの罠セーブ用
		if (Global::trapCameraActivatedInTrap2Map) {
			camera.setTargetCenter(saveTrapCameraPos);
			cameraScale = 2.0;
			latestActivatedTriggerID = specialSaveTrapTriggerID;
		}
		else {
			if (!Global::isLoopStage) {
				camera.setTargetCenter(executeCameraPos());
				cameraScale = 1.0;
			}
		}

		//タイトルカード処理
		titleCard.update();
		achive.update();

		camera.setTargetScale(cameraScale);
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
				b->update();
			}

			//毎フレームGameObjectをspatialGridに登録
			stockNearGameObjects.clear();
			stockBulletsNearGameObjects.clear();
			stockLargeNearGameObjects.clear();

			stockNearGameObjects.add(player.get());
			stockLargeNearGameObjects.add(player.get());

			// トリガーの更新と、最新の起動トリガーIDの取得
			//latestActivatedTriggerID = -1;
			for (auto& t : triggers) {
				stockLargeNearGameObjects.add(t.get());
				//生成時に設定した他条件で起動するトリガー用
				switch (t->getTriggerType()) {
				case TriggerType::Normal:
					//他通常トリガー用
					if (t->getIsActivated()) {
						latestActivatedTriggerID = t->getTrapID();
					}
					break;
				case TriggerType::OtherCondition:
						//他条件トリガー用
						if (t->checkOtherConditionFunc() && t->getIsActivated()) {
							latestActivatedTriggerID = t->getTrapID();
						}
					break;
				}
			}

			// 特殊罠用にトリガー再設定
			if (Global::trapCameraActivatedInTrap2Map)latestActivatedTriggerID = specialSaveTrapTriggerID;

			if ((stageName == U"trap1" || stageName == U"trap2")
				&& latestActivatedTriggerID >= 0
				&& !playedTrapPonTriggerIDs.contains(latestActivatedTriggerID)) {
				bool hasFlyingTrap = false;
				for (const auto& spike : spikes) {
					if (auto* spikeTrap = dynamic_cast<SpikeTrap*>(spike.get())) {
						hasFlyingTrap |= (spikeTrap->getTrapID() == latestActivatedTriggerID);
					}
					if (auto* spikePathTrap = dynamic_cast<SpikePathTrap*>(spike.get())) {
						hasFlyingTrap |= (spikePathTrap->getTrapID() == latestActivatedTriggerID);
					}
				}
				for (const auto& cherry : cherries) {
					if (auto* cherryTrap = dynamic_cast<CherryTrap*>(cherry.get())) {
						hasFlyingTrap |= (cherryTrap->getTrapID() == latestActivatedTriggerID);
					}
				}
				if (hasFlyingTrap) {
					//Sound::playOneShot(Sound::VC_PON);
					playedTrapPonTriggerIDs.insert(latestActivatedTriggerID);
				}
			}

			//playerに現在の罠IDを渡す
			player->setNowTrapID(latestActivatedTriggerID);

			// 対ブロック
			for (auto& b : blocks) {
				stockNearGameObjects.add(b.get());
				stockBulletsNearGameObjects.add(b.get());
				if (b->isTriggerTrap)b->trapUpdate(latestActivatedTriggerID);
			}

			// 針の更新と、起動しているトリガーIDの反映
			for (auto& s : spikes) {
				s->update();
				s->trapUpdate(latestActivatedTriggerID);
				stockNearGameObjects.add(s.get());
			}

			// 特殊罠の更新と、起動しているトリガーIDの反映
			for (auto& st : specialTraps) {
				st->update();
				st->setNowTrapID(latestActivatedTriggerID);
				st->setIsPlayerDied(player->getIsDead());

				if (st->getIsNeedPlayerDir()) {
					st->setDirection(player->pos);
				}

				stockLargeNearGameObjects.add(st.get());
			}
			for (auto& st : specialBackTraps) {
				st->update();
				st->setNowTrapID(latestActivatedTriggerID);
				stockLargeNearGameObjects.add(st.get());
			}

			for (auto& b : bullets) {
				b->update();
			}

			// ----- りんご関連 ----- //
			for (auto& c : cherries) {
				c->update();
				switch (c->getCherryType()){
					case CherryType::Trap: c->trapUpdate(latestActivatedTriggerID); break;
					case CherryType::Gimmik: c->barrageUpdate(); break;
				}
				stockNearGameObjects.add(c.get());
			}
			// 一時格納したりんごをここでまとめて追加
			for (auto& c : pendingCherries) {
				cherries << c;
			}
			pendingCherries.clear();

			for (auto& s : savePoints) {
				s->update();
				if (s->getIsTrap()) {
					s->trapUpdate(latestActivatedTriggerID);
				}
				stockNearGameObjects.add(s.get());
				stockBulletsNearGameObjects.add(s.get());
			}
			//ワープ
			for (auto& w : warps) {
				w->update();
				stockNearGameObjects.add(w.get());
			}
			//看板
			for (auto& s : signs) {
				s->update();
				stockNearGameObjects.add(s.get());
			}

			//アイテム
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

			if (stageName == U"normal4"
				&& Global::isSecretTriggerActivated
				&& !hasSecretEntranceFlashShown) {
				hasSecretEntranceFlashShown = true;
				secretEntranceFlashAlpha = secretEntranceFlashStartAlpha;
			}

			if (secretEntranceFlashAlpha > 0.0) {
				secretEntranceFlashAlpha = Max(0.0, secretEntranceFlashAlpha - secretEntranceFlashFadeSpeed);
			}

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

			//あるマップ専用処理
			if (stageName == U"trap2") {
				if (latestActivatedTriggerID == 30) {
					Global::trapActivatedInTrap2Map = true;
				}
			}

			//暗転演出込みのマップ用
			if (darkEffectStages.includes(stageName)) {
				if (darkAlpha > 0.35) {
					darkAlpha -= 0.05;
				}
				else {
					if (darkAlphaTimer.reachedZero()) {
						darkAlpha = 0.05 + Random(0.30);
						darkAlphaTimer.restart();
					}
				}
			}

			// ----- 以下削除処理 -----

			//画面外のりんごを削除
			cherries.remove_if([](auto&& cherry) {
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

			//役目を終えた特殊罠削除
			specialTraps.remove_if([](auto&& s) {
				return s->isDelete;
			});
			specialBackTraps.remove_if([](auto&& s) {
				return s->isDelete;
			});

			//弾丸削除
			bullets.remove_if([](auto&& bullet) {
				return bullet->isOutOfScreen || bullet->isDelete;
			});
		}
	}

	void StageManager::updateBulletSpikeHits() {
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

	void StageManager::debug() {
		auto& player = gameObjects.player;

		if (Global::inputDebugMuteki.down()) {
			player->setIsMuteki(!player->getIsMuteki());
			Global::recordHaibokusyaModeUseIfNeeded();
		}

		//Print << U" Stage Step : " << step;
		//Print << U" Player Pos : " << player->pos;
		//Print << U" Player Muteki : " << player->getIsMuteki();
		//Print << U" Camera Pos : " << executeCameraPos();
		//Print << U" Cherries Num : " << gameObjects.cherries.size();
		//Print << U" Bullets Num : " << gameObjects.bullets.size();
		//Print << U" Spikes Num : " << gameObjects.spikes.size();
		//Print << U" Special Num : " << gameObjects.specialTraps[0]->pos;
	}

	void StageManager::draw() {
		//背景描画
		if (Global::isExtraStage(stageName)) {
			drawExtraStageBackground(stageName);
		}
		else {
			TextureAsset(backgroundName).draw();
		}

		{
			const auto t = camera.createTransformer();

			//特殊罠描画(後ろ側)
			for (auto it = gameObjects.specialBackTraps.rbegin(); it != gameObjects.specialBackTraps.rend(); ++it) {
				(*it)->draw();
			}
			//針描画
			for (const auto& s : gameObjects.spikes) s->draw();
			//トリガー描画
			for (const auto& t : gameObjects.triggers) t->draw();
			//セーブポイント描画
			for (const auto& s : gameObjects.savePoints) s->draw();
			//ワープの描画
			for (const auto& w : gameObjects.warps) w->draw();
			//看板描画
			for (const auto& s : gameObjects.signs) s->draw();
			//アイテム描画
			for (const auto& i : gameObjects.items) i->draw();
			//kid君描画
			gameObjects.player->draw();
			//ブロック描画
			for (const auto& b : gameObjects.blocks) b->draw();
			//血の描画
			for (const auto& b : gameObjects.bloods) b->draw();
			//弾丸描画
			for (const auto& b : gameObjects.bullets) b->draw();
			//りんご描画
			for (const auto& c : gameObjects.cherries) c->draw();
			//特殊罠描画
			for (const auto& st : gameObjects.specialTraps) st->draw();

			//暗転演出
			if (darkEffectStages.includes(stageName))Rect(0, 0, 800, 608).draw(ColorF(0.0, 0.0, 0.0, darkAlpha));

			//GAMEOVER描画
			if (isShowGameOver) {
				const StringView gameOverTextureName = (Global::mainTextureNumber == 0) ? U"sprGAMEOVER_low" : U"sprGAMEOVER_normal";
				if (Global::trapCameraActivatedInTrap2Map) {
					TextureAsset(gameOverTextureName).scaled(1 / cameraScale).drawAt(saveTrapCameraPos);
				}
				else if (Global::isLoopStage)
				{
					TextureAsset(gameOverTextureName).drawAt(Global::stageWidth / 2, Global::stageHeight / 2);
				}
				else {
					TextureAsset(gameOverTextureName).drawAt(executeCameraPos());
				}
			}

			//タイトルカード
			titleCard.draw();
			achive.draw();
		}

		if (secretEntranceFlashAlpha > 0.0) {
			Rect{ 0, 0, Global::windowWidth, Global::windowHeight }.draw(ColorF{ 1.0, 1.0, 1.0, secretEntranceFlashAlpha });
		}

		if (stageName == U"clear") {
			FontAsset(U"Button")(U"Escでメニュー").draw(36, 36, ColorF{ 0.0, 0.0, 0.0 });

			const Vec2 basePos{ 400, 388 };
			const String timeText = U"Time  " + formatPlayTime(Global::elapsedPlayTime);
			const String deathText = U"Death " + Format(Global::deathCount);

			FontAsset(U"BossHp")(timeText).drawAt(basePos + Vec2{ 2, 2 }, ColorF{ 0.0, 0.0, 0.0, 0.65 });
			FontAsset(U"BossHp")(deathText).drawAt(basePos + Vec2{ 2, 48 }, ColorF{ 0.0, 0.0, 0.0, 0.65 });
			FontAsset(U"BossHp")(timeText).drawAt(basePos, ColorF{ 1.0, 1.0, 1.0 });
			FontAsset(U"BossHp")(deathText).drawAt(basePos + Vec2{ 0, 46 }, ColorF{ 1.0, 1.0, 1.0 });
		}

		if (stageName == U"tutorialLow") {
			const Vec2 basePos{ 50, 50 };
			const Array<String> tutorialTexts{
				U"←→キー : 移動",
				U"shift : ジャンプ、二段ジャンプ",
				U"Zキー : ショット",
				U"ctrl : 無敵"
			};

			for (int32 i = 0; i < tutorialTexts.size(); ++i) {
				const Vec2 textPos = basePos + Vec2{ 0, i * 26.0 };
				FontAsset(U"Button")(tutorialTexts[i]).draw(textPos + Vec2{ 1, 1 }, ColorF{ 0.5, 0.7 });
				FontAsset(U"Button")(tutorialTexts[i]).draw(textPos, ColorF{ 0.0, 0.0, 0.0 });
			}
		}
	}

	void StageManager::setStep(int32 newStep) {
		step = newStep;
	}

	// セーブ処理
	void StageManager::saveGame() {
		//プレイヤーの位置を保存
		Global::savedStartPlayerPos = gameObjects.player->pos;
		Global::savedRoomName = stageName;
		Global::savedIsWarpMode = Global::canUseItem2Effect() && gameObjects.player->getIsWarpMode();
		Global::isExistSaveData = true;
	}

	// カメラの位置をプレイヤーのいるエリアの中心に設定
	Vec2 StageManager::executeCameraPos() {
		const double halfWindowWidth = Global::windowWidth / 2.0;
		const double halfWindowHeight = Global::windowHeight / 2.0;
		const double maxPlayerX = Max(0.0, Global::stageWidth - 1.0);
		const double maxPlayerY = Max(0.0, Global::stageHeight - 1.0);
		const Vec2 clampedPlayerPos{
			Clamp(gameObjects.player->pos.x, 0.0, maxPlayerX),
			Clamp(gameObjects.player->pos.y, 0.0, maxPlayerY)
		};

		const int32 playerAreaX = static_cast<int32>(clampedPlayerPos.x) / Global::windowWidth;
		const int32 playerAreaY = static_cast<int32>(clampedPlayerPos.y) / Global::windowHeight;
		Vec2 nextPos{
			playerAreaX * Global::windowWidth + halfWindowWidth,
			playerAreaY * Global::windowHeight + halfWindowHeight
		};

		nextPos.x = Clamp(nextPos.x, halfWindowWidth, Max(halfWindowWidth, Global::stageWidth - halfWindowWidth));
		nextPos.y = Clamp(nextPos.y, halfWindowHeight, Max(halfWindowHeight, Global::stageHeight - halfWindowHeight));
		return nextPos;
	}

	std::shared_ptr<Player> StageManager::getPlayer() {
		return gameObjects.player;
	}

	Array<std::shared_ptr<Cherry>> StageManager::getCherries() {
		return gameObjects.cherries;
	}

	Array<std::shared_ptr<Block>> StageManager::getBlocks() {
		return gameObjects.blocks;
	}

	String StageManager::getStageName() const {
		return stageName;
	}

	//ある罠用に取得用
	std::shared_ptr<SpecialTrap> StageManager::getWarningWindowTrap() {
		for (auto& st : gameObjects.specialTraps) {
			if (auto trap = std::dynamic_pointer_cast<WarningWindowTrap>(st)) {
				return trap;
			}
		}
		return nullptr;
	}

	//りんご生成と管理配列への追加
	void StageManager::createCherry(std::shared_ptr<Cherry> cherry) {
		pendingCherries << cherry;
	}

	void StageManager::createPlayerWarpEffectCherries(Vec2 centerPos) {
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
	void StageManager::createPeripheryBlocks() {
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
	void StageManager::createFloorBlocks(Vec2 basePos) {
		gameObjects.blocks << std::make_shared<Block>(U"sprBlock", Vec2(basePos.x, basePos.y));
		for (int i = 1; i <= 3; i++) {
			gameObjects.blocks << std::make_shared<Block>(U"sprFloor", Vec2(basePos.x + i, basePos.y));
		}
		gameObjects.blocks << std::make_shared<Block>(U"sprBlock", Vec2(basePos.x + 4, basePos.y));
	}
}
