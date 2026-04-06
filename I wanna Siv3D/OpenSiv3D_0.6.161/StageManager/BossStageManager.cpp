#include "BossStageManager.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	BossStageManager::BossStageManager() {
		stockNearGameObjects.cellSize = 96;
		stockBulletsNearGameObjects.cellSize = 160;
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

		Global::bgmStop = false;
		Global::trapActivatedId30InTrap2Map = false;
		Global::trapCameraActivatedInTrap2Map = false;
		Global::isPlayerFrozen = false;
		Global::isBossAttackPowerUp = false;

		gameoverTimer.reset();
		isShowGameOver = false;

		if(Global::isChangeRoom)loadGameObjects(Global::nowRoomName);
		else loadGameObjects(Global::savedRoomName);
		Global::isChangeRoom = false;
	}

	void BossStageManager::loadGameObjects(String fileName) {
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
				//case 25: gameObjects.savePoints << std::make_shared<SavePoint>(pos); break;
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
			String gimmikString;

			if (stage.contains(U"Gimmiks")) {
				for (const auto& gimmik : stage[U"Gimmiks"].arrayView()) {
					gimmikName = gimmik[U"gimmikName"].getString();
					gimmikParsePos = parsePos(gimmik[U"gimmikPos"]);
					gimmikIntactPos = parseIntactPos(gimmik[U"gimmikPos"]);

					if (gimmikName == U"ワープ") {
						gimmikString = gimmik[U"value1"].getString();
						gimmikValue2 = gimmik[U"value2"].get<double>();
						gimmikValue3 = gimmik[U"value3"].get<double>();
					}
					else {
						gimmikValue1 = gimmik[U"value1"].get<double>();
						gimmikValue2 = gimmik[U"value2"].get<double>();
						gimmikValue3 = gimmik[U"value3"].get<double>();
					}

					if (gimmikName == U"ワープ") gameObjects.warps << std::make_shared<Warp>(gimmikIntactPos, gimmikString);
				}
			}
		}

		if (stageName == U"boss") {
			gameObjects.savePoints << std::make_shared<BossSavePoint>(Vec2{400,500},1);
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

		if (player->getIsDead()) {
			gameoverTimer.start();
			if (gameoverTimer.reachedZero()) {
				isShowGameOver = true;
			}
		}

		// 揺れ更新
		cameraShake.update();
		// カメラ位置 + 揺れ
		camera.setTargetCenter(executeCameraPos() + cameraShake.getOffset());
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

			// 対ブロック
			for (auto& b : blocks) {
				stockNearGameObjects.add(b.get());
				stockBulletsNearGameObjects.add(b.get());
			}

			// 針の更新と、起動しているトリガーIDの反映
			for (auto& s : spikes) {
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

				if (bc->getCherryType() == CherryType::Boss) {
					auto* b = dynamic_cast<BossCherry*>(bc.get());
					bossCherryPos = b->pos;
					defeatedBossNum = b->getDefeatedBossNum();
					attackCherryType = b->getBossCherryAttackType();
				}
				else if (bc->getCherryType() == CherryType::BossSub) {
					auto* bs = dynamic_cast<BossSubCherry*>(bc.get());
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
				s->onSavedCallback = [this,s]() {
					generateBoss(s->getAppendBossId());
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

			// 倒されたボスりんごのサブりんご取得用処理
			if (!bossCherries.empty()) {
				auto* bossCherry = dynamic_cast<BossCherry*>(bossCherries.front().get());
				for (auto& bc : bossCherries) {
					if (bc->getCherryType() == CherryType::BossSub) {
						auto* bs = dynamic_cast<BossSubCherry*>(bc.get());
						if (bs->isDelete) bossCherry->removeDefeatedAttackType(bs->getBossCherrySubType());
					}
				}
			}

			//暗転演出の透明度を変更
			if (darkAlpha > 0.1) {
				darkAlpha -= 0.08;
			}
			else {
				if (darkAlphaTimer.reachedZero()) {
					darkAlpha = 0.05 + Random(0.05);
					darkAlphaTimer.restart();
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
		Rect(0, 0, 800, 608).draw(ColorF(0.8, 1.0));

		camera.update(); {
			const auto t = camera.createTransformer();

			//針描画
			for (auto s : gameObjects.spikes) s->draw();
			//ブロック描画
			for (auto b : gameObjects.blocks) b->draw();
			//セーブポイント描画
			for (auto s : gameObjects.savePoints) s->draw();
			//ワープの描画
			for (auto w : gameObjects.warps) w->draw();
			//ボスりんご描画
			for (auto it = gameObjects.bossCherries.rbegin(); it != gameObjects.bossCherries.rend(); ++it) {
				(*it)->draw();
			}
			//りんご描画
			for (auto c : gameObjects.cherries) c->draw();
			//kid君描画
			gameObjects.player->draw();
			//血の描画
			for (auto b : gameObjects.bloods) b->draw();
			//弾丸描画
			for (auto b : gameObjects.bullets) b->draw();

			//暗転演出
			Rect(0, 0, 800, 608).draw(ColorF(0.0, 0.0, 0.0, darkAlpha));

			//GAMEOVER描画
			if(isShowGameOver) TextureAsset(U"sprGAMEOVER").drawAt(executeCameraPos());
		}
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

	String BossStageManager::getStageName() const {
		return stageName;
	}

	//りんご生成と管理配列への追加
	void BossStageManager::createCherry(std::shared_ptr<Cherry> cherry) {
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
