#include "StageManager.h"

namespace Iwanna {
	void StageManager::loadGameObjects(String fileName) {
		//ステージデータの読み込みとオブジェクト生成
		CSV csv{ U"MapData/" + fileName + U".csv" };

		if (!csv)
		{
			throw Error{ U"次のCSVが読み込めません : " + fileName + U".csv" };
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

				if (fileName == U"secret1") {
					switch (value) {
					case 1: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_secret1", pos); break;
					case 6: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_secret2", pos); break;
					case 7: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_secret3", pos); break;
					case 21: gameObjects.spikes << std::make_shared<Spike>(U"secret", pos, 0); break;
					case 22: gameObjects.spikes << std::make_shared<Spike>(U"secret", pos, 1); break;
					case 23: gameObjects.spikes << std::make_shared<Spike>(U"secret", pos, 2); break;
					case 24: gameObjects.spikes << std::make_shared<Spike>(U"secret", pos, 3); break;
					case 25: gameObjects.savePoints << std::make_shared<SecretSavePoint>(pos,U"normal5"); break;
					case 26: gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_low1", pos); break;
					case 27: gameObjects.blocks << std::make_shared<ShootTroughBlock>(U"sprBlockShootTrough", pos); break;
					case 28: gameObjects.blocks << std::make_shared<FakeBlock>(U"sprBlock_low2", pos); break;
					case 29: gameObjects.blocks << std::make_shared<WaterBlock>(U"sprWater", pos); break;
					case 31: gameObjects.cherries << std::make_shared<SpriteCherry>(U"sprCherryLowWhiteLine", pos, 1); break;
					case 36: gameObjects.spikes << std::make_shared<AppendSpike>(U"secret", pos, 0); break;
					case 37: gameObjects.spikes << std::make_shared<AppendSpike>(U"secret", pos, 2); break;
					case 38: gameObjects.spikes << std::make_shared<DeleteSpike>(U"secret", pos, 0); break;
					case 39: gameObjects.spikes << std::make_shared<DeleteSpike>(U"secret", pos, 2); break;
					case 40: gameObjects.triggers << std::make_shared<SecretTrigger>(pos); break;
					case 43: gameObjects.spikes << std::make_shared<DeleteSpike>(U"secret", pos, 1); break;
					case 44: gameObjects.spikes << std::make_shared<DeleteSpike>(U"secret", pos, 3); break;
					}

					Global::isLoopStage = true;
				}
				else {
					// value に応じて配置
					switch (value) {
					case 1: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_low1", pos); break;
					case 6: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_low2", pos); break;
					case 7: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_low3", pos); break;
					case 21: gameObjects.spikes << std::make_shared<Spike>(U"low", pos, 0); break;
					case 22: gameObjects.spikes << std::make_shared<Spike>(U"low", pos, 1); break;
					case 23: gameObjects.spikes << std::make_shared<Spike>(U"low", pos, 2); break;
					case 24: gameObjects.spikes << std::make_shared<Spike>(U"low", pos, 3); break;
					case 25: gameObjects.savePoints << std::make_shared<SavePoint>(pos); break;
					case 26: gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_low1", pos); break;
					case 27: gameObjects.blocks << std::make_shared<ShootTroughBlock>(U"sprBlockShootTrough", pos); break;
					case 28: gameObjects.blocks << std::make_shared<FakeBlock>(U"sprBlock_low2", pos); break;
					case 29: gameObjects.blocks << std::make_shared<WaterBlock>(U"sprWater", pos); break;
					case 31: gameObjects.cherries << std::make_shared<SpriteCherry>(U"sprCherryLow", pos, 1); break;
					case 36: gameObjects.spikes << std::make_shared<AppendSpike>(U"low", pos, 0); break;
					case 37: gameObjects.spikes << std::make_shared<AppendSpike>(U"low", pos, 2); break;
					case 38: gameObjects.spikes << std::make_shared<DeleteSpike>(U"low", pos, 0); break;
					case 39: gameObjects.spikes << std::make_shared<DeleteSpike>(U"low", pos, 2); break;
					case 40: gameObjects.triggers << std::make_shared<SecretTrigger>(pos); break;
					case 43: gameObjects.spikes << std::make_shared<DeleteSpike>(U"low", pos, 1); break;
					case 44: gameObjects.spikes << std::make_shared<DeleteSpike>(U"low", pos, 3); break;
					}

					Global::isLoopStage = false;
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
			Vec2 startPlayerPos = parseIntactPos(stage[U"startPlayerPos"]);

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

					//特定マップの特定idのトラップ用
					if (fileName == U"trap1") {
						if (gimmikValue1 == 3 && gimmikName == U"罠針_下") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 2, static_cast<int32>(gimmikValue1), Vec2{ 0,4 }, 2.0);
							continue;
						}
						if (gimmikValue1 == 5 && gimmikName == U"罠針_左") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 1, static_cast<int32>(gimmikValue1), Vec2{ -30,0 }, 2.0);
							continue;
						}
						if (gimmikValue1 == 7 && gimmikName == U"罠針_左") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 1, static_cast<int32>(gimmikValue1), Vec2{ -13,0 }, 0.7);
							continue;
						}
						if (gimmikValue1 == 11 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<WarningWindowTrap>(Vec2{ 400,304 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 12) {
							if (gimmikName == U"罠針_上") {
								gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ 0,-3 }, 0.5);
								continue;
							}
							else if (gimmikName == U"罠針_右") {
								gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 3, static_cast<int32>(gimmikValue1), Vec2{ 0,-3 }, 0.5);
								continue;
							}
						}
						if (gimmikValue1 == 13 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<SteamTrap>(Vec2{ 688,670 }, static_cast<int32>(gimmikValue1));
						}
					}

					// ----- 罠マップ2マップ目 ----- //
					if (fileName == U"trap2") {
						if (gimmikValue1 == 2 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<TitleTrap>(Vec2{ 145,-32 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 3 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<TitleTrap2>(Vec2{ 70,-32 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 4 && gimmikName == U"罠針_上") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ 0,-1 }, 0.1);
							continue;
						}
						if (gimmikValue1 == 11 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<AdWindowTrap>(Vec2{ 480,448 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 12 && gimmikName == U"罠トリガー") {
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_low1", Vec2{ 512,576 }, static_cast<int32>(gimmikValue1));
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_low1", Vec2{ 544,576 }, static_cast<int32>(gimmikValue1));
							gameObjects.specialTraps << std::make_shared<DiscordTrap>(Vec2{ 912, 534 }, static_cast<int32>(gimmikValue1));
							gameObjects.specialTraps << std::make_shared<DiscordCherryTrap>(Vec2{ 825, 544 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 24 && gimmikName == U"前トリガー") {
							gameObjects.specialBackTraps << std::make_shared<TreeTrap>(Vec2{ 400,80 }, static_cast<int32>(gimmikValue1), U"treeTrap");
						}
						if (gimmikValue1 == 26 && gimmikName == U"前トリガー") {
							gameObjects.specialBackTraps << std::make_shared<TreeTrap>(Vec2{ 400,80 }, static_cast<int32>(gimmikValue1), U"transParentTreeTrap");
						}
						if (gimmikValue1 == 28 && gimmikName == U"前トリガー") {
							gameObjects.savePoints << std::make_shared<SaveMoveTrap>(Vec2{ 384,384 }, static_cast<int32>(gimmikValue1), 18, 90);
						}
						if (gimmikValue1 == 29 && gimmikName == U"前トリガー") {
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_low1", Vec2{ 424,240 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 30 && gimmikName == U"罠ブロック") {
							gameObjects.blocks << std::make_shared<BreakBlock>(U"sprBlock_low3", gimmikParsePos, static_cast<int32>(gimmikValue1));
						}
						//専用の隠しブロック
						if (gimmikValue1 == 32 && Global::trapActivatedInTrap2Map && !Global::trapActivatedId30InTrap2Map) {
							for (int32 i = 0; i < 5; i++) gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_low1", Vec2{ 19 + i,7 });
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
							gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ 2,0 }, 0.1);
							continue;
						}
						if (gimmikValue1 == 34 && gimmikName == U"前トリガー") {
							gameObjects.triggers << std::make_shared<Trigger>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3, [this]() {return Global::trapActivatedInTrap2Map && !Global::trapActivatedId30InTrap2Map; });
							continue;
						}
						if (gimmikValue1 == 35 && gimmikName == U"前トリガー") {
							gameObjects.savePoints << std::make_shared<SaveMoveTrap>(Vec2{ 704,128 }, static_cast<int32>(gimmikValue1), 8, 270);
						}
					}

					// ----- 水色りんごギミックマップ ----- //
					if (fileName == U"normal5") {
						if (gimmikName == U"罠針_上") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ -1,1 }, 0.5);
							continue;
						}
					}

					// ギミックの種類に応じてオブジェクトを生成
					if (gimmikName == U"罠針_上") gameObjects.spikes << std::make_shared<SpikeTrap>(U"low", gimmikParsePos, 0, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
					if (gimmikName == U"罠針_左") gameObjects.spikes << std::make_shared<SpikeTrap>(U"low", gimmikParsePos, 1, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
					if (gimmikName == U"罠針_下") gameObjects.spikes << std::make_shared<SpikeTrap>(U"low", gimmikParsePos, 2, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
					if (gimmikName == U"罠針_右") gameObjects.spikes << std::make_shared<SpikeTrap>(U"low", gimmikParsePos, 3, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
					if (gimmikName == U"移動針_上") gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ gimmikValue2, gimmikValue3 }, gimmikValue4);
					if (gimmikName == U"移動針_左") gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 1, static_cast<int32>(gimmikValue1), Vec2{ gimmikValue2, gimmikValue3 }, gimmikValue4);
					if (gimmikName == U"移動針_下") gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 2, static_cast<int32>(gimmikValue1), Vec2{ gimmikValue2, gimmikValue3 }, gimmikValue4);
					if (gimmikName == U"移動針_右") gameObjects.spikes << std::make_shared<SpikePathTrap>(U"low", gimmikParsePos, 3, static_cast<int32>(gimmikValue1), Vec2{ gimmikValue2, gimmikValue3 }, gimmikValue4);
					if (gimmikName == U"罠トリガー") gameObjects.triggers << std::make_shared<Trigger>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3, false);
					if (gimmikName == U"前トリガー") gameObjects.triggers << std::make_shared<Trigger>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3, true);
					if (gimmikName == U"罠りんご") gameObjects.cherries << std::make_shared<CherryTrap>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
					if (gimmikName == U"罠ブロック") gameObjects.blocks << std::make_shared<BreakBlock>(U"sprBlock_low3", gimmikParsePos, static_cast<int32>(gimmikValue1));
					if (gimmikName == U"ワープ") gameObjects.warps << std::make_shared<Warp>(gimmikIntactPos, gimmikString);
					if (gimmikName == U"特殊ワープ") gameObjects.warps << std::make_shared<SecretWarp>(gimmikIntactPos, gimmikString);
					if (gimmikName == U"昇降針") gameObjects.spikes << std::make_shared<SpikeUpDown>(U"low", gimmikIntactPos,static_cast<int32>(gimmikValue1),gimmikValue2);
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

			gameObjects.specialTraps << std::make_shared<MouseTrap>(Vec2{ Cursor::PosF() });

			gameObjects.warps << std::make_shared<Warp>(Vec2{ 1600,512 }, U"boss");
			gameObjects.warps << std::make_shared<Warp>(Vec2{ 1600,544 }, U"boss");

			//画面外ブロック
			gameObjects.blocks << std::make_shared<Block>(U"sprBlock_low1", Vec2{ 16,-1 });
			gameObjects.blocks << std::make_shared<Block>(U"sprBlock_low1", Vec2{ 17,-1 });
		}

		// ----- 以下通常ステージのギミック ----- //

		if (fileName == U"normal2")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{400,304},3.0,CherryColorType::Red,*this);
		if (fileName == U"normal3")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{400,560},3.0,CherryColorType::Blue,*this);
		if (fileName == U"normal4")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{400,304},3.0,CherryColorType::Yellow,*this);
		if (fileName == U"normal5")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{80,80},3.0,CherryColorType::Sky,*this);
		if (fileName == U"normal6")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{80,528},3.0,CherryColorType::Green,*this);
		if (fileName == U"normal7")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{400,304},3.0,CherryColorType::Orange,*this);
	}
}
