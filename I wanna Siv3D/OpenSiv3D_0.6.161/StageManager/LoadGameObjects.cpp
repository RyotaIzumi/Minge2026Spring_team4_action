#include "StageManager.h"

namespace Iwanna {
	namespace {
		CherryColorType getExtraStageCherryColor(const String& fileName) {
			if (fileName == U"ExMiluArea") return CherryColorType::White;
			if (fileName == U"ExMochiArea") return CherryColorType::Orange;
			if (fileName == U"ExGotArea") return CherryColorType::Gray;
			if (fileName == U"ExRyutaArea") return CherryColorType::Sky;
			return CherryColorType::None;
		}

		ColorF getExtraStageBlockColor(const String& fileName) {
			if (fileName == U"ExMiluArea") return ColorF{ Palette::White };
			if (fileName == U"ExMochiArea") return ColorF{ Palette::Orange };
			if (fileName == U"ExGotArea") return ColorF{ Palette::Gray };
			if (fileName == U"ExRyutaArea") return ColorF{ 0.32, 0.62, 1.0 };
			return ColorF{ Palette::White };
		}

		void applyExtraCherryVisual(const std::shared_ptr<Cherry>& cherry, const String& fileName) {
			if (!Global::isExtraStage(fileName)) {
				return;
			}

			cherry->setCherryVisual(U"sprCherryLowWhite", getExtraStageCherryColor(fileName), true);
		}
	}

	void StageManager::loadGameObjects(String fileName) {
		String quarity;
		const bool isExtraStage = Global::isExtraStage(fileName);
		const bool isTrapMap = (fileName == U"trap1" || fileName == U"trap2" || fileName == U"trapBoss");
		const bool isTrapPonMap = (fileName == U"trap1" || fileName == U"trap2");

		switch (Global::mainTextureNumber) {
		case 0: quarity = U"low"; break;
		case 1: quarity = U"normal"; break;
		case 2: quarity = U"extra"; break;
		}
		if (isExtraStage) {
			quarity = U"extra";
		}
		const String spikeTextureType = isExtraStage ? U"extra" : isTrapPonMap ? U"trap" : quarity;

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
					case 26: gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_" + quarity + U"1", pos); break;
					case 27: gameObjects.blocks << std::make_shared<ShootTroughBlock>(U"sprBlockShootTrough", pos); break;
					case 28: gameObjects.blocks << std::make_shared<FakeBlock>(U"sprBlock_" + quarity + U"2", pos); break;
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
					Global::doNotStopBgm = true;
				}
				else {
					// value に応じて配置
					switch (value) {
					case 1: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_" + quarity + U"1", pos); break;
					case 6: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_" + quarity + U"2", pos); break;
					case 7: gameObjects.blocks << std::make_shared<Block>(U"sprBlock_" + quarity + U"3", pos); break;
					case 21: gameObjects.spikes << std::make_shared<Spike>(spikeTextureType, pos, 0); break;
					case 22: gameObjects.spikes << std::make_shared<Spike>(spikeTextureType, pos, 1); break;
					case 23: gameObjects.spikes << std::make_shared<Spike>(spikeTextureType, pos, 2); break;
					case 24: gameObjects.spikes << std::make_shared<Spike>(spikeTextureType, pos, 3); break;
					case 25: gameObjects.savePoints << std::make_shared<SavePoint>(pos); break;
					case 26: gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_" + quarity + U"1", pos); break;
					case 27: gameObjects.blocks << std::make_shared<ShootTroughBlock>(isExtraStage ? U"sprBlock_extra2" : U"sprBlockShootTrough", pos); break;
					case 28: gameObjects.blocks << std::make_shared<FakeBlock>(U"sprBlock_" + quarity + U"2", pos); break;
					case 29: gameObjects.blocks << std::make_shared<WaterBlock>(U"sprWater", pos); break;
					case 31:
					{
						auto cherry = std::make_shared<SpriteCherry>(isTrapMap ? U"sprCherryTrap" : U"sprCherryLow", pos, 1);
						applyExtraCherryVisual(cherry, fileName);
						gameObjects.cherries << cherry;
						break;
					}
					case 36: gameObjects.spikes << std::make_shared<AppendSpike>(spikeTextureType, pos, 0); break;
					case 37: gameObjects.spikes << std::make_shared<AppendSpike>(spikeTextureType, pos, 2); break;
					case 38: gameObjects.spikes << std::make_shared<DeleteSpike>(spikeTextureType, pos, 0); break;
					case 39: gameObjects.spikes << std::make_shared<DeleteSpike>(spikeTextureType, pos, 2); break;
					case 40: gameObjects.triggers << std::make_shared<SecretTrigger>(pos); break;
					case 43: gameObjects.spikes << std::make_shared<DeleteSpike>(spikeTextureType, pos, 1); break;
					case 44: gameObjects.spikes << std::make_shared<DeleteSpike>(spikeTextureType, pos, 3); break;
					}

					Global::isLoopStage = false;
					Global::doNotStopBgm = false;
				}
			}
		}

		if (isExtraStage) {
			Global::doNotStopBgm = true;
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
							gameObjects.spikes << std::make_shared<SpikePathTrap>(spikeTextureType, gimmikParsePos, 2, static_cast<int32>(gimmikValue1), Vec2{ 0,4 }, 2.0);
							continue;
						}
						if (gimmikValue1 == 5 && gimmikName == U"罠針_左") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(spikeTextureType, gimmikParsePos, 1, static_cast<int32>(gimmikValue1), Vec2{ -30,0 }, 2.0);
							continue;
						}
						if (gimmikValue1 == 7 && gimmikName == U"罠針_左") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(spikeTextureType, gimmikParsePos, 1, static_cast<int32>(gimmikValue1), Vec2{ -13,0 }, 0.7);
							continue;
						}
						if (gimmikValue1 == 11 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<WarningWindowTrap>(Vec2{ 400,304 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 12) {
							if (gimmikName == U"罠針_上") {
								gameObjects.spikes << std::make_shared<SpikePathTrap>(spikeTextureType, gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ 0,-3 }, 0.5);
								continue;
							}
							else if (gimmikName == U"罠針_右") {
								gameObjects.spikes << std::make_shared<SpikePathTrap>(spikeTextureType, gimmikParsePos, 3, static_cast<int32>(gimmikValue1), Vec2{ 0,-3 }, 0.5);
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
							gameObjects.spikes << std::make_shared<SpikePathTrap>(spikeTextureType, gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ 0,-1 }, 0.1);
							continue;
						}
						if (gimmikValue1 == 11 && gimmikName == U"罠トリガー") {
							gameObjects.specialTraps << std::make_shared<AdWindowTrap>(Vec2{ 480,448 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 12 && gimmikName == U"罠トリガー") {
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_" + quarity + U"1", Vec2{ 512,576 }, static_cast<int32>(gimmikValue1));
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_" + quarity + U"1", Vec2{ 544,576 }, static_cast<int32>(gimmikValue1));
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
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_" + quarity + U"1", Vec2{ 424,240 }, static_cast<int32>(gimmikValue1));
						}
						if (gimmikValue1 == 30 && gimmikName == U"罠ブロック") {
							gameObjects.blocks << std::make_shared<BreakBlock>(U"sprBlock_" + quarity + U"3", gimmikParsePos, static_cast<int32>(gimmikValue1));
						}
						//専用の隠しブロック
						if (gimmikValue1 == 32 && Global::trapActivatedInTrap2Map && !Global::trapActivatedId30InTrap2Map) {
							for (int32 i = 0; i < 5; i++) gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_" + quarity + U"1", Vec2{ 19 + i,7 });
							gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_" + quarity + U"1", Vec2{ 23,8 });
							gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_" + quarity + U"1", Vec2{ 23,9 });
							for (int32 i = 0; i < 3; i++) gameObjects.blocks << std::make_shared<HideBlock>(U"sprBlock_" + quarity + U"1", Vec2{ 14,7 + i });
						}
						if (gimmikValue1 == 32 && gimmikName == U"前トリガー") {
							gameObjects.triggers << std::make_shared<Trigger>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3, [this]() {return Global::trapActivatedInTrap2Map && !Global::trapActivatedId30InTrap2Map; });
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_" + quarity + U"1", Vec2{ 608,256 }, static_cast<int32>(gimmikValue1));
							gameObjects.blocks << std::make_shared<ConditionalHideBlock>(U"sprBlock_" + quarity + U"1", Vec2{ 608,288 }, static_cast<int32>(gimmikValue1));
							continue;
						}
						if (gimmikValue1 == 33 && gimmikName == U"罠針_上") {
							gameObjects.spikes << std::make_shared<SpikePathTrap>(quarity, gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ 2,0 }, 0.1);
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
							gameObjects.spikes << std::make_shared<SpikePathTrap>(spikeTextureType, gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ -1,1 }, 0.5);
							continue;
						}
					}

					// ギミックの種類に応じてオブジェクトを生成
					if (gimmikName == U"罠針_上") {
						auto spike = std::make_shared<SpikeTrap>(spikeTextureType, gimmikIntactPos, 0, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
						gameObjects.spikes << spike;
					}
					if (gimmikName == U"罠針_左") {
						auto spike = std::make_shared<SpikeTrap>(spikeTextureType, gimmikIntactPos, 1, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
						gameObjects.spikes << spike;
					}
					if (gimmikName == U"罠針_下") {
						auto spike = std::make_shared<SpikeTrap>(spikeTextureType, gimmikIntactPos, 2, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
						gameObjects.spikes << spike;
					}
					if (gimmikName == U"罠針_右") {
						auto spike = std::make_shared<SpikeTrap>(spikeTextureType, gimmikIntactPos, 3, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
						gameObjects.spikes << spike;
					}
					if (gimmikName == U"移動針_上") {
						auto spike = std::make_shared<SpikePathTrap>(spikeTextureType, gimmikParsePos, 0, static_cast<int32>(gimmikValue1), Vec2{ gimmikValue2, gimmikValue3 }, gimmikValue4);
						gameObjects.spikes << spike;
					}
					if (gimmikName == U"移動針_左") {
						auto spike = std::make_shared<SpikePathTrap>(spikeTextureType, gimmikParsePos, 1, static_cast<int32>(gimmikValue1), Vec2{ gimmikValue2, gimmikValue3 }, gimmikValue4);
						gameObjects.spikes << spike;
					}
					if (gimmikName == U"移動針_下") {
						auto spike = std::make_shared<SpikePathTrap>(spikeTextureType, gimmikParsePos, 2, static_cast<int32>(gimmikValue1), Vec2{ gimmikValue2, gimmikValue3 }, gimmikValue4);
						gameObjects.spikes << spike;
					}
					if (gimmikName == U"移動針_右") {
						auto spike = std::make_shared<SpikePathTrap>(spikeTextureType, gimmikParsePos, 3, static_cast<int32>(gimmikValue1), Vec2{ gimmikValue2, gimmikValue3 }, gimmikValue4);
						gameObjects.spikes << spike;
					}
					if (gimmikName == U"罠トリガー") gameObjects.triggers << std::make_shared<Trigger>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3, false);
					if (gimmikName == U"前トリガー") gameObjects.triggers << std::make_shared<Trigger>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3, true);
					if (gimmikName == U"罠りんご") {
						auto cherry = std::make_shared<CherryTrap>(gimmikIntactPos, static_cast<int32>(gimmikValue1), gimmikValue2, gimmikValue3);
						applyExtraCherryVisual(cherry, fileName);
						gameObjects.cherries << cherry;
					}
					if (gimmikName == U"罠ブロック") gameObjects.blocks << std::make_shared<BreakBlock>(U"sprBlock_" + quarity + U"3", gimmikParsePos, static_cast<int32>(gimmikValue1));
					if (gimmikName == U"時間罠ブロック") gameObjects.blocks << std::make_shared<TimedBreakBlock>(U"sprBlock_" + quarity + U"3", gimmikParsePos, static_cast<int32>(gimmikValue1), gimmikValue2);
					if (gimmikName == U"ワープ") gameObjects.warps << std::make_shared<Warp>(gimmikIntactPos, gimmikString);
					if (gimmikName == U"特殊ワープ") gameObjects.warps << std::make_shared<SecretWarp>(gimmikIntactPos, gimmikString);
					if (gimmikName == U"昇降針") gameObjects.spikes << std::make_shared<SpikeUpDown>(spikeTextureType, gimmikIntactPos,static_cast<int32>(gimmikValue1),gimmikValue2);
					if (gimmikName == U"ループ移動針") gameObjects.spikes << std::make_shared<SpikeLoopMove>(spikeTextureType, gimmikIntactPos, static_cast<int32>(gimmikValue1), Vec2{ gimmikValue2, gimmikValue3 }, gimmikValue4);
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

			gameObjects.warps << std::make_shared<Warp>(Vec2{ 1600,512 }, U"trapBoss");
			gameObjects.warps << std::make_shared<Warp>(Vec2{ 1600,544 }, U"trapBoss");

			//画面外ブロック
			gameObjects.blocks << std::make_shared<Block>(U"sprBlock_" + quarity + U"1", Vec2{ 16,-1 });
			gameObjects.blocks << std::make_shared<Block>(U"sprBlock_" + quarity + U"1", Vec2{ 17,-1 });
		}

		// ----- 以下通常ステージのギミック ----- //

		if (fileName == U"tutorial" || fileName == U"tutorialTrap") {
			gameObjects.signs << std::make_shared<Sign>(Vec2{ 3,17 }, SignType::Tutorial_Move);
			gameObjects.signs << std::make_shared<Sign>(Vec2{ 8,17 }, SignType::Tutorial_Jump);
			gameObjects.signs << std::make_shared<Sign>(Vec2{ 12,16 }, SignType::Tutorial_DoubleJump);
			gameObjects.signs << std::make_shared<Sign>(Vec2{ 18,15 }, SignType::Tutorial_AdjustJump);
			gameObjects.signs << std::make_shared<Sign>(Vec2{ 27,17 }, SignType::Tutorial_Shoot);
			gameObjects.signs << std::make_shared<Sign>(Vec2{ 30,16 }, SignType::Tutorial_Save);
			gameObjects.signs << std::make_shared<Sign>(Vec2{ 37,17 }, SignType::Tutorial_Spike);
			gameObjects.signs << std::make_shared<Sign>(Vec2{ 61,7 }, SignType::Tutorial_Water);
			gameObjects.signs << std::make_shared<Sign>(Vec2{ 62,17 }, SignType::Tutorial_Item);
		}

		if (fileName == U"secret1" && !Global::getItem1) {
			gameObjects.items << std::make_shared<Item>(Vec2{7,9},ItemType::Heart);
		}

		if (fileName == U"normal2")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{400,304},3.0,CherryColorType::Red,*this);
		if (fileName == U"normal3")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{400,560},3.0,CherryColorType::Blue,*this);
		if (fileName == U"normal4")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{400,304},3.0,CherryColorType::Yellow,*this);
		if (fileName == U"normal5")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{80,80},3.0,CherryColorType::Sky,*this);
		if (fileName == U"normal6")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{80,528},3.0,CherryColorType::Green,*this);
		if (fileName == U"normal7")gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{624,304},3.0,CherryColorType::Orange,*this);
		if (fileName == U"ExRyutaArea") {
			const double grayBigCherryScale = 7.0;
			gameObjects.cherries << std::make_shared<GimmikBigCherry>(Vec2{400,304}, grayBigCherryScale, CherryColorType::Gray, *this);
		}

		if (isExtraStage) {
			const ColorF blockColor = getExtraStageBlockColor(fileName);
			for (auto& block : gameObjects.blocks) {
				if (block->blockType != BlockType::Water) {
					block->setBlockColor(blockColor);
				}
			}
		}
	}
}
