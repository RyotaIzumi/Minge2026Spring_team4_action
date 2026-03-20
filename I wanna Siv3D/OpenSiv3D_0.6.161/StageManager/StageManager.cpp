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

		// 一部変数の初期化
		isGenerateBloods = false;

		Global::bgmStop = false;
		Global::trapActivatedId30InTrap2Map = false;
		Global::trapCameraActivatedInTrap2Map = false;
		Global::isPlayerFrozen = false;

		gameoverTimer.reset();
		isShowGameOver = false;

		if (!Global::isExistSaveData)Global::savedRoomName = Global::startRoomName;

		if(Global::isChangeRoom)loadGameObjects(Global::nowRoomName);
		else loadGameObjects(Global::savedRoomName);
		Global::isChangeRoom = false;
	}

	void StageManager::loadGameObjects(String fileName) {
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
				case 1: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_low1", pos); break;
				case 6: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_low2", pos); break;
				case 7: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_low3", pos); break;
				case 21: gameObjects.spikes << std::make_shared<Spike>(pos, 0); break;
				case 22: gameObjects.spikes << std::make_shared<Spike>(pos, 1); break;
				case 23: gameObjects.spikes << std::make_shared<Spike>(pos, 2); break;
				case 24: gameObjects.spikes << std::make_shared<Spike>(pos, 3); break;
				case 25: gameObjects.savePoints << std::make_shared<SavePoint>(pos); break;
				case 26: gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_low1", pos); break;
				case 27: gameObjects.blocks << std::make_shared<ShootTroughBlock>(U"sprBlockShootTrough", pos); break;
				case 28: gameObjects.blocks << std::make_shared<FakeBlock>(U"sprBlock_low2", pos); break;
				}
			}
		}

		//ステージサイズを更新
		Global::stageWidth = csv.columns(0) * oneTileSize;
		Global::stageHeight = csv.rows() * oneTileSize;

		stageName = fileName;
		latestActivatedTriggerID = -1;

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

			String gimmikName;
			Vec2 gimmikParsePos;
			Vec2 gimmikIntactPos;
			double gimmikValue1;
			double gimmikValue2;
			double gimmikValue3;

			if (stage.contains(U"Gimmiks")) {
				for (const auto& gimmik : stage[U"Gimmiks"].arrayView()) {
					gimmikName = gimmik[U"gimmikName"].getString();
					gimmikParsePos = parsePos(gimmik[U"gimmikPos"]);
					gimmikIntactPos = parseIntactPos(gimmik[U"gimmikPos"]);
					gimmikValue1 = gimmik[U"value1"].get<double>();
					gimmikValue2 = gimmik[U"value2"].get<double>();
					gimmikValue3 = gimmik[U"value3"].get<double>();

					//特定マップの特定idのトラップ用
					if (fileName == U"trap1") {
						if(gimmikValue1 == 3 && gimmikName == U"罠針_下"){
							gameObjects.spikes << std::make_shared<SpikePathTrap>(gimmikParsePos, 2, static_cast<int32>(gimmikValue1), Vec2{0,4},2.0);
							continue;
						}
						if (gimmikValue1 == 5 && gimmikName == U"罠針_左") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(gimmikParsePos, 1, static_cast<int32>(gimmikValue1), Vec2{ -30,0 }, 2.0);
							continue;
						}
						if (gimmikValue1 == 7 && gimmikName == U"罠針_左") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(gimmikParsePos, 1, static_cast<int32>(gimmikValue1), Vec2{ -13,0 }, 0.7);
							continue;
						}
						if (gimmikValue1 == 11 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<WarningWindowTrap>(Vec2{ 400,304 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 12) {
							if (gimmikName == U"罠針_上") {
								gameObjects.spikes << std::make_shared<SpikePathTrap>(gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ 0,-3 }, 0.5);
								continue;
							}
							else if (gimmikName == U"罠針_右") {
								gameObjects.spikes << std::make_shared<SpikePathTrap>(gimmikParsePos, 3, static_cast<int32>(gimmikValue1), Vec2{ 0,-3 }, 0.5);
								continue;
							}
						}
						if (gimmikValue1 == 13 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<SteamTrap>(Vec2{ 688,670 }, static_cast<int32>(gimmikValue1));
						}
					}

					//罠マップ2マップ目
					if (fileName == U"trap2") {
						if (gimmikValue1 == 2 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<TitleTrap>(Vec2{ 145,-32 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 3 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<TitleTrap2>(Vec2{ 70,-32 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 4 && gimmikName == U"罠針_上") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ 0,-1 }, 0.1);
							continue;
						}
						if (gimmikValue1 == 11 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<AdWindowTrap>(Vec2{ 480,448 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 12 && gimmikName == U"罠トリガー") {
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_low1", Vec2{ 512,576 }, static_cast<int32>(gimmikValue1));
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_low1", Vec2{ 544,576 }, static_cast<int32>(gimmikValue1));
							gameObjects.specialTraps << std::make_shared<DiscordTrap>(Vec2{912, 534}, static_cast<int32>(gimmikValue1));
							gameObjects.specialTraps << std::make_shared<DiscordCherryTrap>(Vec2{825, 544}, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 24 && gimmikName == U"前トリガー") {
							gameObjects.specialBackTraps << std::make_shared<TreeTrap>(Vec2{ 400,80 }, static_cast<int32>(gimmikValue1), U"treeTrap");
						}
						if (gimmikValue1 == 26 && gimmikName == U"前トリガー") {
							gameObjects.specialBackTraps << std::make_shared<TreeTrap>(Vec2{ 400,80 }, static_cast<int32>(gimmikValue1), U"transParentTreeTrap");
						}
						if (gimmikValue1 == 28 && gimmikName == U"前トリガー") {
							gameObjects.savePoints << std::make_shared<SaveMoveTrap>(Vec2{ 384,384 }, static_cast<int32>(gimmikValue1),18,90);
						}
						if (gimmikValue1 == 29 && gimmikName == U"前トリガー") {
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_low1", Vec2{ 424,240 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 30 && gimmikName == U"罠ブロック") {
							gameObjects.blocks << std::make_shared<BreakBlock>(U"sprBlock_low3",gimmikParsePos, static_cast<int32>(gimmikValue1));
						}
						//専用の隠しブロック
						if (gimmikValue1 == 32 && Global::trapActivatedInTrap2Map && !Global::trapActivatedId30InTrap2Map) {
							for(int32 i = 0;i < 5;i++) gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_low1", Vec2{ 19 + i,7 });
							gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_low1", Vec2{ 23,8 });
							gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_low1", Vec2{ 23,9 });
							for (int32 i = 0; i < 3; i++) gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_low1", Vec2{ 14,7 + i });
						}
						if (gimmikValue1 == 32 && gimmikName == U"前トリガー") {
							gameObjects.triggers << std::make_shared<Trigger>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3, [this]() {return Global::trapActivatedInTrap2Map && !Global::trapActivatedId30InTrap2Map; });
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_low1", Vec2{ 608,256 }, static_cast<int32>(gimmikValue1));
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_low1", Vec2{ 608,288 }, static_cast<int32>(gimmikValue1));
							continue;
						}
						if (gimmikValue1 == 33 && gimmikName == U"罠針_上") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ 2,0 }, 0.1);
							continue;
						}
						if (gimmikValue1 == 34 && gimmikName == U"前トリガー") {
							gameObjects.triggers << std::make_shared<Trigger>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3, [this]() {return Global::trapActivatedInTrap2Map && !Global::trapActivatedId30InTrap2Map; });
							continue;
						}
						if (gimmikValue1 == 35 && gimmikName == U"前トリガー") {
							gameObjects.savePoints << std::make_shared<SaveMoveTrap>(Vec2{ 704,128 }, static_cast<int32>(gimmikValue1),8,270);
						}
					}

					// ギミックの種類に応じてオブジェクトを生成
					if (gimmikName == U"罠針_上") gameObjects.spikes << std::make_shared<SpikeTrap>(gimmikParsePos, 0, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
					if (gimmikName == U"罠針_左") gameObjects.spikes << std::make_shared<SpikeTrap>(gimmikParsePos, 1, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
					if (gimmikName == U"罠針_下") gameObjects.spikes << std::make_shared<SpikeTrap>(gimmikParsePos, 2, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
					if (gimmikName == U"罠針_右") gameObjects.spikes << std::make_shared<SpikeTrap>(gimmikParsePos, 3, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
					if (gimmikName == U"罠トリガー") gameObjects.triggers << std::make_shared<Trigger>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3, false);
					if (gimmikName == U"前トリガー") gameObjects.triggers << std::make_shared<Trigger>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3, true);
					if (gimmikName == U"罠りんご") gameObjects.cherries << std::make_shared<CherryTrap>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
				}
			}
		}

		// トリガー不必要の特殊配置物
		//(Warpは後々Editに適応)

		if (fileName == U"trap1") {
			gameObjects.warps << std::make_shared<Warp>(Vec2{ 512,608 }, U"trap2");
			gameObjects.warps << std::make_shared<Warp>(Vec2{ 544,608 }, U"trap2");
		}

		if (fileName == U"trap2") {
			if (Global::trapActivatedInTrap2Map)latestActivatedTriggerID = 30;

			saveTrapCameraPos = { 928,320 };
			gameObjects.savePoints << std::make_shared<SaveFakeTrap>(Vec2{ saveTrapCameraPos });
			gameObjects.specialTraps << std::make_shared<PanddTrap>(Vec2{ saveTrapCameraPos.x, saveTrapCameraPos.y + 800 }, specialSaveTrapTriggerID);

			gameObjects.specialTraps << std::make_shared<MouseTrap>(Vec2{Cursor::PosF()});

			gameObjects.warps << std::make_shared<Warp>(Vec2{ 1600,512 }, U"boss");
			gameObjects.warps << std::make_shared<Warp>(Vec2{ 1600,544},U"boss");

			//画面外ブロック
			gameObjects.blocks << std::make_shared<Block>(U"sprBlock_low1", Vec2{16,-1});
			gameObjects.blocks << std::make_shared<Block>(U"sprBlock_low1", Vec2{17,-1});
		}
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
			camera.setTargetCenter(executeCameraPos());
			cameraScale = 1.0;
		}

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
				stockNearGameObjects.add(b.get());
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
				if (t->getCheckOtherCondition()) {
					if (t->checkOtherConditionFunc() && t->getIsActivated()) {
						latestActivatedTriggerID = t->getTrapID();
					}
					continue;
				}
				//他通常トリガー用
				if (t->getIsActivated()) {
					latestActivatedTriggerID = t->getTrapID();
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
			for (auto& c : cherries) {
				c->update();
				if (c->isTrap)c->trapUpdate(latestActivatedTriggerID);
				stockNearGameObjects.add(c.get());
			}
			for (auto& s : savePoints) {
				s->update();
				if (s->getIsTrap()) {
					s->trapUpdate(latestActivatedTriggerID);
					stockNearGameObjects.add(s.get());
				}
				s->onSavedCallback = [this]() {
					saveGame();
				};
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

			//あるマップ専用処理
			if (stageName == U"trap2") {
				if (latestActivatedTriggerID == 30) {
					Global::trapActivatedInTrap2Map = true;
				}
			}

			// ----- 以下削除処理 -----

			//画面外のりんごを削除
			cherries.remove_if([](auto&& cherry) {
				return cherry->isOutOfScreen;
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

		/*
		ClearPrint();
		Print << U" Stage Step : " << step;
		Print << U" Player Pos : " << player->pos;
		Print << U" Player Muteki : " << player->getIsMuteki();
		Print << U" Camera Pos : " << executeCameraPos();
		Print << U" Cherries Num : " << gameObjects.cherries.size();
		Print << U" Bullets Num : " << gameObjects.bullets.size();
		Print << U" Spikes Num : " << gameObjects.spikes.size();
		Print << U" Special Num : " << gameObjects.specialTraps[0]->pos;
		*/
	}

	void StageManager::draw() {
		//背景描画
		Rect(0, 0, 800, 608).draw(ColorF(0.8, 1.0));

		camera.update(); {
			const auto t = camera.createTransformer();

			//特殊罠描画(後ろ側)
			for (auto it = gameObjects.specialBackTraps.rbegin(); it != gameObjects.specialBackTraps.rend(); ++it) {
				(*it)->draw();
			}
			//針描画
			for (auto s : gameObjects.spikes) s->draw();
			//ブロック描画
			for (auto b : gameObjects.blocks) b->draw();
			//トリガー描画
			for (auto t : gameObjects.triggers) t->draw();
			//セーブポイント描画
			for (auto s : gameObjects.savePoints) s->draw();
			//ワープの描画
			for (auto w : gameObjects.warps) w->draw();
			//kid君描画
			gameObjects.player->draw();
			//血の描画
			for (auto b : gameObjects.bloods) b->draw();
			//弾丸描画
			for (auto b : gameObjects.bullets) b->draw();
			//りんご描画
			for (auto c : gameObjects.cherries) c->draw();
			//特殊罠描画
			for (auto st : gameObjects.specialTraps) st->draw();

			//GAMEOVER描画
			if(isShowGameOver)
				if(Global::trapCameraActivatedInTrap2Map) TextureAsset(U"sprGAMEOVER").scaled(1 / cameraScale).drawAt(saveTrapCameraPos);
				else TextureAsset(U"sprGAMEOVER").drawAt(executeCameraPos());
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
		gameObjects.cherries << cherry;
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
