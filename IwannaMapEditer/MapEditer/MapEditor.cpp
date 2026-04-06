#include "MapEditor.h"
#include "TileUtils.h"

// カーソル位置からタイルのインデックスを取得する関数
Optional<Point> GetCursorIndex(const Size& size, int32 tileSize, const Point& offset) {
	const Point cursorPos = (Cursor::Pos() - offset);
	if ((not InRange(cursorPos.x, 0, (size.x * tileSize - 1))) || (not InRange(cursorPos.y, 0, (size.y * tileSize - 1)))) {
		return none;
	}
	return{ cursorPos / tileSize };
}

// タイルIDからタイルのインデックスを取得する関数
int32 GetAutoTileIndexById(const Array<AutoTile> tiles, int32 id) {
	for (size_t i = 0; i < tiles.size(); i++) {
		if (tiles[i].getTileId() == id) {
			return i;
		}
	}
	return 0;
}

MapEditor::MapEditor()
{
	const FilePath objectPath = U"texture/object/";

	autoTiles =
	{
		AutoTile{ Image{ objectPath + U"sprBlock.png" }, 1 },
		AutoTile{ Image{ objectPath + U"sprFloor.png" }, 2 },
		AutoTile{ Image{ objectPath + U"sprFloor2.png" }, 3 },
		AutoTile{ Image{ objectPath + U"sprFloor3.png" }, 4 },
		AutoTile{ Image{ objectPath + U"sprFloor4.png" }, 5 },
		AutoTile{ Image{ objectPath + U"sprFloor5.png" }, 6 },
		AutoTile{ Image{ objectPath + U"sprFloor6.png" }, 7 },
		AutoTile{ Image{ objectPath + U"sprSpikeUp.png" }, 21 },
		AutoTile{ Image{ objectPath + U"sprSpikeLeft.png" }, 22 },
		AutoTile{ Image{ objectPath + U"sprSpikeDown.png" }, 23 },
		AutoTile{ Image{ objectPath + U"sprSpikeRight.png" }, 24 },
		AutoTile{ Image{ objectPath + U"sprSave.png" }, 25 },
		AutoTile{ Image{ objectPath + U"sprBlockHide.png" }, 26 },
		AutoTile{ Image{ objectPath + U"sprBlockShootTrough.png" }, 27 },
		AutoTile{ Image{ objectPath + U"sprBlockFake.png" }, 28 },
	};

	state.mapWidthText.text = Format(state.mapSize.x);
	state.mapHeightText.text = Format(state.mapSize.y);

	playerXText.text = Format(startPlayerPos.x);
	playerYText.text = Format(startPlayerPos.y);
}

void MapEditor::update()
{
	updateMapSize();
	updatePageScroll();
	updateInput();

	const int tileSize = autoTiles[0].getTileSize();

	if (state.settingMode == 0)
	{
		updateTilePlacement();
	}
	else
	{
		gimmikManager.update(state.cursorIndex, tileSize);
	}
}

void MapEditor::updateInput()
{
	const int tileSize = autoTiles[0].getTileSize();

	state.cursorIndex = GetCursorIndexSafe(
		Size{ VIEW_WIDTH, VIEW_HEIGHT },
		tileSize,
		OFFSET,
		state.mapSize,
		state.scrollX,
		state.scrollY
	);

	// Player X
	if (SimpleGUI::TextBox(playerXText, Vec2{ 1100, 620 }, 80))
	{
		if (const auto v = ParseIntOpt<int32>(playerXText.text))
		{
			startPlayerPos.x = *v;
		}
	}

	// Player Y
	if (SimpleGUI::TextBox(playerYText, Vec2{ 1100, 660 }, 80))
	{
		if (const auto v = ParseIntOpt<int32>(playerYText.text))
		{
			startPlayerPos.y = *v;
		}
	}
}

void MapEditor::updateMapSize()
{
	if (const auto w = ParseIntOpt<int32>(state.mapWidthText.text))
	{
		state.mapSize.x = Clamp(*w, 1, 200);
	}

	if (const auto h = ParseIntOpt<int32>(state.mapHeightText.text))
	{
		state.mapSize.y = Clamp(*h, 1, 200);
	}

	// サイズ変更検知
	if (state.gridSize != state.mapSize)
	{
		state.gridSize = state.mapSize;

		state.grid = Grid<uint32>(state.gridSize, 0);
		state.connectivity = Grid<AutoTileConnectivity>(state.gridSize);

		// スクロールリセット
		state.scrollX = 0;
		state.scrollY = 0;
	}
}

void MapEditor::updatePageScroll()
{
	constexpr int VIEW_W = 25;
	constexpr int VIEW_H = 19;

	// ←
	if (SimpleGUI::Button(U"←", Vec2{ 40, 670 }))
	{
		state.scrollX = Max(0.0, state.scrollX - VIEW_W);
	}

	// →
	if (SimpleGUI::Button(U"→", Vec2{ 140, 670 }))
	{
		state.scrollX = Min(
			(double)(state.mapSize.x - VIEW_W),
			state.scrollX + VIEW_W
		);
	}

	// ↑
	if (SimpleGUI::Button(U"↑", Vec2{ 240, 670 }))
	{
		state.scrollY = Max(0.0, state.scrollY - VIEW_H);
	}

	// ↓
	if (SimpleGUI::Button(U"↓", Vec2{ 340, 670 }))
	{
		state.scrollY = Min(
			(double)(state.mapSize.y - VIEW_H),
			state.scrollY + VIEW_H
		);
	}
}

void MapEditor::updateTilePlacement()
{
	if (!state.cursorIndex) return;

	if (MouseL.pressed())
	{
		state.grid[*state.cursorIndex] = state.selectedTileId;
	}
	else if (MouseR.pressed())
	{
		state.grid[*state.cursorIndex] = 0;
		state.connectivity[*state.cursorIndex] = AutoTileConnectivity{};
	}
}

void MapEditor::draw()
{
	drawMap();

	drawMapSizeUI();
	drawPageInfo();
	drawPlayerUI();
	drawPlayerOnMap(autoTiles[0].getTileSize());

	//配置モード切替
	SimpleGUI::RadioButtons( state.settingMode,{ U"ブロック", U"ギミック" },Vec2{ 900, 40 });

	const int tileSize = autoTiles[0].getTileSize();
	if (state.settingMode == 0)
	{
		drawTileSelector(tileSize);
	}
	else
	{
		gimmikManager.draw(tileSize, state.scrollX, state.scrollY);
	}

	// ----- マップ保存関連 ----- //
	FontAsset(U"Font")(U"保存ファイル名").draw(1080, 120);
	SimpleGUI::TextBox(saveFileName, Vec2{ 1080, 150 }, 150);

	//csvの保存と読み込み
	FontAsset(U"Font")(U"CSV").draw(1080, 190);
	if (SimpleGUI::Button(U"Save", Vec2{ 1080, 220 }))
	{
		const FilePath path = mainActionProjectPath + saveFileName.text + U".csv";

		MapSerializer::SaveCSV(state.grid, path);
	}
	if (SimpleGUI::Button(U"Load", Vec2{ 1170, 220 }))
	{
		const FilePath path = mainActionProjectPath + saveFileName.text + U".csv";

		if (!FileSystem::Exists(path)) return;
		CSV csv(path);

		// === サイズ取得 ===
		int32 newHeight = csv.rows();
		int32 newWidth = csv.columns(0);

		// === サイズ反映 ===
		state.mapSize = Size{ newWidth, newHeight };
		state.gridSize = state.mapSize;

		state.grid = Grid<uint32>(state.gridSize, 0);
		state.connectivity = Grid<AutoTileConnectivity>(state.gridSize);

		MapSerializer::LoadCSV(state.grid, path);

		// === UI更新 ===
		state.mapWidthText.text = Format(state.mapSize.x);
		state.mapHeightText.text = Format(state.mapSize.y);

		// === スクロールリセット ===
		state.scrollX = 0;
		state.scrollY = 0;
	}

	// JSONの保存と読み込み
	FontAsset(U"Font")(U"JSON").draw(1080, 260);
	if (SimpleGUI::Button(U"Save", Vec2{ 1080, 290 }))
	{
		const FilePath path = mainActionProjectPath + saveFileName.text + U".json";

		MapSerializer::SaveJSON(
			startPlayerPos,
			gimmikManager.getGimmiks(),
			path
		);
	}
	if (SimpleGUI::Button(U"Load", Vec2{ 1170, 290 }))
	{
		const FilePath path = mainActionProjectPath + saveFileName.text + U".json";

		auto& gimmiks = gimmikManager.getGimmiks();

		MapSerializer::LoadJSON(
			startPlayerPos,
			gimmiks,
			path
		);

		gimmikManager.setGimmiks(gimmiks);

		playerXText.text = Format(startPlayerPos.x);
		playerYText.text = Format(startPlayerPos.y);
	}

	drawCursor();
}

void MapEditor::drawMap()
{
	const int tileSize = autoTiles[0].getTileSize();

	int startX = state.scrollX;
	int startY = state.scrollY;

	int endX = Min(startX + VIEW_WIDTH, state.mapSize.x);
	int endY = Min(startY + VIEW_HEIGHT, state.mapSize.y);

	for (int y = startY; y < endY; ++y)
	{
		for (int x = startX; x < endX; ++x)
		{
			Point drawPos = Point{ x - startX, y - startY } * tileSize + OFFSET;

			baseTexture.draw(drawPos);

			if (state.grid[y][x])
			{
				//描画対象のタイルを取得する
				const auto targetTile = autoTiles[GetAutoTileIndexById(autoTiles,state.grid[y][x])];
				int32 targetId = targetTile.getTileId();

				targetTile.getTile(targetId, 0).draw(drawPos, state.settingMode == 0 ? ColorF{ 1.0, 1.0 } : ColorF{ 1.0, 0.5 });
			}
			else
			{
				Rect{ drawPos, tileSize }.drawFrame(1, ColorF{ 0.5 });
			}
		}
	}
}

void MapEditor::drawMapSizeUI()
{
	FontAsset(U"Font")(U"マップサイズ").draw(1080, 10);

	FontAsset(U"Font")(U"Width").draw(1080, 40);
	FontAsset(U"Font")(U"High").draw(1180, 40);

	SimpleGUI::TextBox(state.mapWidthText, Vec2{ 1080, 70 }, 80);
	SimpleGUI::TextBox(state.mapHeightText, Vec2{ 1180, 70 }, 80);
}

void MapEditor::drawPlayerUI()
{
	FontAsset(U"Font")(U"Player初期座標").draw(1100, 580);

	FontAsset(U"Font")(U"x").draw(1080, 625);
	FontAsset(U"Font")(U"y").draw(1080, 665);
}

void MapEditor::drawPlayerOnMap(int tileSize)
{
	Vec2 pos = startPlayerPos
		- Vec2{ state.scrollX * tileSize, state.scrollY * tileSize }
	+ Vec2{ 40, 40 }; // LayerOffset

	Circle{ pos, 10 }.draw(Palette::Yellow);
}

void MapEditor::drawPageInfo()
{
	constexpr int VIEW_W = 25;
	constexpr int VIEW_H = 19;

	int pageX = state.scrollX / VIEW_W;
	int pageY = state.scrollY / VIEW_H;

	FontAsset(U"Font")(U"Page: ({} , {})"_fmt(pageX, pageY)).draw(450, 670);
}

void MapEditor::drawCursor()
{
	if (!state.cursorIndex) return;

	const int tileSize = autoTiles[0].getTileSize();

	const int posX = static_cast<int>((state.cursorIndex->x - state.scrollX) * tileSize);
	const int posY = static_cast<int>((state.cursorIndex->y - state.scrollY) * tileSize);

	Point pos{ posX, posY };

	pos += OFFSET;

	Rect{ pos, tileSize }.draw(ColorF{ 1.0, 0.5, 0.0, 0.5 });
}

// タイル選択の描画
void MapEditor::drawTileSelector(int32 tileSize)
{
	// タイル選択枠の位置を変えたい場合はこれを編集する
	constexpr Point OFFSET{ 900, 140 };

	int32 autoTilesValue = 0;
	int32 tileCounter = 1;

	for (int32 y = 0; y < tileGrid.height(); ++y)
	{
		for (int32 x = 0; x < tileGrid.width(); ++x)
		{
			Point pos = Point{ x, y } * tileSize + OFFSET;

			if (autoTilesValue < autoTiles.size()
				&& autoTiles[autoTilesValue].getTileId() == tileCounter)
			{
				autoTiles[autoTilesValue]
					.getTile(tileCounter, 0)
					.draw(pos);

				autoTilesValue++;
			}

			Rect{ pos, tileSize }.drawFrame(1, ColorF{ 1.0 });
			tileCounter++;
		}
	}

	// カーソル位置取得
	const auto touched =
		GetCursorIndex(tileGrid.size(), tileSize, OFFSET);

	// ホバー表示
	if (touched)
	{
		Rect{ (*touched * tileSize + OFFSET), tileSize }
		.drawFrame(2, Palette::Blue);
	}

	// クリック選択
	if (touched && MouseL.down())
	{
		selectedTileCursorIndex = touched;

		state.selectedTileId =
			(touched->y * tileGrid.width() + touched->x + 1);

		// 存在チェック
		bool found = false;
		for (const auto& t : autoTiles)
		{
			if (t.getTileId() == state.selectedTileId)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			state.selectedTileId = 0;
		}
	}

	// 選択中表示
	Rect{ (*selectedTileCursorIndex * tileSize + OFFSET), tileSize }
	.drawFrame(2, Palette::Red);
}
