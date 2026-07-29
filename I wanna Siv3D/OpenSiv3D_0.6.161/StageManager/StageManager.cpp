#include "StageManager.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
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

			if (Global::prepareGetItem1) {
				achive.startShowAchieve(AchieveType::ItemGet_Heart);
				Global::prepareGetItem1 = false;
				Global::getItem1 = true;
			}
		}
		else {
			loadGameObjects(Global::savedRoomName);
		}
		Global::isChangeRoom = false;

		// セーブ時コールバックは、生成直後に一度だけ設定する
		for (auto& savePoint : gameObjects.savePoints) {
			savePoint->onSavedCallback = [this]() {
				saveGame();
			};
		}

		//アイテム入手関連
		Global::prepareGetItem1 = false;
	}

	Vec2 StageManager::parsePos(const JSON& json) {
		return Vec2{ json[0].get<int32>() / oneTileSize, json[1].get<int32>() / oneTileSize };
	}

	Vec2 StageManager::parseIntactPos(const JSON& json) {
		return Vec2{ json[0].get<int32>(), json[1].get<int32>()};
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
					bullets << std::make_shared<Bullet>(player->pos, player->getDirection() == Global::Direction::RIGHT ? bulletSpeed : -bulletSpeed);
					AudioAsset(Sound::SHOOT).playOneShot();
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

			//あるマップ専用処理
			if (stageName == U"trap2") {
				if (latestActivatedTriggerID == 30) {
					Global::trapActivatedInTrap2Map = true;
				}
			}

			//暗転演出込みのマップ用
			if (darkEffectStages.includes(stageName)) {
				if (darkAlpha > 0.3) {
					darkAlpha -= 0.05;
				}
				else {
					if (darkAlphaTimer.reachedZero()) {
						darkAlpha = 0.05 + Random(0.20);
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

	void StageManager::debug() {
		auto& player = gameObjects.player;

		if (Global::inputDebugMuteki.down()) {
			player->setIsMuteki(!player->getIsMuteki());
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
		TextureAsset(backgroundName).draw();

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
				if (Global::trapCameraActivatedInTrap2Map) {
					TextureAsset(U"sprGAMEOVER_normal").scaled(1 / cameraScale).drawAt(saveTrapCameraPos);
				}
				else if (Global::isLoopStage)
				{
					TextureAsset(U"sprGAMEOVER_normal").drawAt(Global::stageWidth / 2, Global::stageHeight / 2);
				}
				else {
					TextureAsset(U"sprGAMEOVER_normal").drawAt(executeCameraPos());
				}
			}

			//タイトルカード
			titleCard.draw();
			achive.draw();
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
		Global::isExistSaveData = true;
	}

	// カメラの位置をプレイヤーのいるエリアの中心に設定
	Vec2 StageManager::executeCameraPos() {
		Vec2 nextPos;
		int32 playerAreaX = static_cast<int32>(gameObjects.player->pos.x) / Global::windowWidth;
		int32 playerAreaY = static_cast<int32>(gameObjects.player->pos.y) / Global::windowHeight;
		nextPos.x = playerAreaX * Global::windowWidth + Global::windowWidth / 2;
		nextPos.y = playerAreaY * Global::windowHeight + Global::windowHeight / 2;
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
