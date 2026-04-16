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
		AutoTile{ Image{ objectPath + U"sprWater.png" }, 29 },
		AutoTile{ Image{ objectPath + U"sprCherryLow.png" }, 31 },
		AutoTile{ Image{ objectPath + U"sprSpikeUpAppend.png" }, 36 },
		AutoTile{ Image{ objectPath + U"sprSpikeDownAppend.png" }, 37 },
		AutoTile{ Image{ objectPath + U"sprSpikeUpDelete.png" }, 38 },
		AutoTile{ Image{ objectPath + U"sprSpikeDownDelete.png" }, 39 },
		AutoTile{ Image{ objectPath + U"sprSecretTrigger.png" }, 40 },
		AutoTile{ Image{ objectPath + U"sprSpikeLeftDelete.png" }, 43 },
		AutoTile{ Image{ objectPath + U"sprSpikeRightDelete.png" }, 44 }
	};

	crossTileIds =
	{
		31
	};

	state.mapWidthText.text = Format(state.mapSize.x);
	state.mapHeightText.text = Format(state.mapSize.y);

	playerXText.text = Format(startPlayerPos.x);
	playerYText.text = Format(startPlayerPos.y);

	LoadBackgroundList(mainActionProjectBackgroundPath,backgroundNames,backgroundListBox);
}

void MapEditor::LoadBackgroundList(const FilePath& folder,Array<String>& names,ListBoxState& listBox){
	names.clear();
	listBox.items.clear();

	for (const auto& path : FileSystem::DirectoryContents(folder))
	{
		if (FileSystem::IsFile(path))
		{
			// 拡張子チェック（画像のみ）
			String ext = FileSystem::Extension(path).lowercased();

			if (ext == U"png")
			{
				// ファイル名だけ取得
				String name = FileSystem::BaseName(path);

				names << name;
				listBox.items << name;
			}
		}
	}

	for (const auto& name : backgroundNames)
	{
		FilePath path = mainActionProjectBackgroundPath + name + U".png";
		backgroundTextures[name] = Texture{ path };
	}
}

void MapEditor::update()
{
	updateUndoRedo();
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
		if (MouseL.down()) saveSnapshot();
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

	if (MouseL.down() || MouseR.down()) saveSnapshot();

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

void MapEditor::updateUndoRedo()
{
	if (KeyControl.pressed())
	{
		if (KeyZ.down())
		{
			undo();
		}
		else if (KeyY.down())
		{
			redo();
		}
	}
}

void MapEditor::draw()
{
	drawBackground();
	drawMap();

	drawMapSizeUI();
	drawPageInfo();
	drawPlayerOnMap(autoTiles[0].getTileSize());

	//配置モード切替
	SimpleGUI::RadioButtons( state.settingMode,{ U"ブロック", U"ギミック" },Vec2{ 900, 40 });

	const int tileSize = autoTiles[0].getTileSize();
	if (state.settingMode == 0)
	{
		drawTileSelector(tileSize);
		drawPlayerUI();
		drawBackgroundUI();
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
	if (SimpleGUI::Button(U"Save", Vec2{ 1080, 220 },unspecified,canSaveFile()))
	{
		saveSnapshot();
		const FilePath path = mainActionProjectMapDataPath + saveFileName.text + U".csv";

		MapSerializer::SaveCSV(state.grid, path);
	}
	if (SimpleGUI::Button(U"Load", Vec2{ 1170, 220 }))
	{
		saveSnapshot();
		const FilePath path = mainActionProjectMapDataPath + saveFileName.text + U".csv";

		if (!FileSystem::Exists(path)) {
			Print << U"csvファイル : " + path + U"が見つかりません!";
			return;
		}
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
	if (SimpleGUI::Button(U"Save", Vec2{ 1080, 290 },unspecified,canSaveFile()))
	{
		saveSnapshot();
		const FilePath path = mainActionProjectMapDataPath + saveFileName.text + U".json";

		MapSerializer::SaveJSON(
			startPlayerPos,
			currentBackground,
			gimmikManager.getGimmiks(),
			path
		);
	}
	if (SimpleGUI::Button(U"Load", Vec2{ 1170, 290 }))
	{
		saveSnapshot();
		const FilePath path = mainActionProjectMapDataPath + saveFileName.text + U".json";

		auto& gimmiks = gimmikManager.getGimmiks();

		MapSerializer::LoadJSON(
			startPlayerPos,
			currentBackground,
			gimmiks,
			path
		);

		gimmikManager.setGimmiks(gimmiks);

		playerXText.text = Format(startPlayerPos.x);
		playerYText.text = Format(startPlayerPos.y);

		// 背景情報更新
		backgroundListBox.selectedItemIndex = none;
		for (size_t i = 0; i < backgroundNames.size(); ++i)
		{
			if (backgroundNames[i] == currentBackground)
			{
				backgroundListBox.selectedItemIndex = i;
				break;
			}
		}
	}

	if (KeyC.down())ClearPrint();

	drawCursor();
}

void MapEditor::drawMap()
{
	const int tileSize = autoTiles[0].getTileSize();
	const Vec2 crossOffset = Vec2{ tileSize, tileSize } / 2;

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

				if(crossTileIds.contains(targetId)) targetTile.getTile(targetId, 0).draw(drawPos - crossOffset, state.settingMode == 0 ? ColorF{ 1.0, 1.0 } : ColorF{ 1.0, 0.5 });
				else targetTile.getTile(targetId, 0).draw(drawPos, state.settingMode == 0 ? ColorF{ 1.0, 1.0 } : ColorF{ 1.0, 0.5 });
			}
			else
			{
				Rect{ drawPos, tileSize }.drawFrame(1, ColorF{ 0.5 });
			}
		}
	}
}

void MapEditor::drawBackground() {
	if (backgroundTextures.contains(currentBackground)){
		backgroundTextures[currentBackground].draw(OFFSET);
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
	Vec2 basePos{ 1100, 400 };

	FontAsset(U"Font")(U"Player初期座標").draw(basePos);

	FontAsset(U"Font")(U"x").draw(basePos.x, basePos.y + 40);
	FontAsset(U"Font")(U"y").draw(basePos.x + 85, basePos.y + 40);

	// Player X
	if (SimpleGUI::TextBox(playerXText, Vec2{ basePos.x + 20, basePos.y + 40 }, 60))
	{
		if (const auto v = ParseIntOpt<int32>(playerXText.text))
		{
			startPlayerPos.x = *v;
		}
	}

	// Player Y
	if (SimpleGUI::TextBox(playerYText, Vec2{ basePos.x + 100, basePos.y + 40 }, 60))
	{
		if (const auto v = ParseIntOpt<int32>(playerYText.text))
		{
			startPlayerPos.y = *v;
		}
	}
}

void MapEditor::drawBackgroundUI()
{
	Vec2 basePos{ 1100, 500 };
	FontAsset(U"Font")(U"背景").draw(basePos);

	SimpleGUI::ListBox(backgroundListBox, Vec2{ basePos.x,basePos.y + 40 }, 120, 160);

	if (!backgroundListBox.selectedItemIndex) return;
	currentBackground = backgroundListBox.items[*backgroundListBox.selectedItemIndex];
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

// ----- undo / redo 関連 ----- //

void MapEditor::saveSnapshot()
{
	EditorSnapshot snap;

	snap.grid = state.grid;
	snap.gimmiks = gimmikManager.getGimmiks();
	snap.playerPos = startPlayerPos;

	undoStack << snap;

	// 上限
	if (undoStack.size() > MAX_HISTORY)
	{
		undoStack.pop_front();
	}

	// 新操作時はRedo消す
	redoStack.clear();
}

void MapEditor::loadSnapshot(const EditorSnapshot& snap)
{
	state.grid = snap.grid;
	gimmikManager.getGimmiks() = snap.gimmiks;
	startPlayerPos = snap.playerPos;
}

void MapEditor::undo()
{
	if (undoStack.isEmpty()) return;

	// 現在をredoへ
	EditorSnapshot current{
		state.grid,
		gimmikManager.getGimmiks(),
		startPlayerPos
	};

	redoStack << current;

	// 1つ戻る
	auto snap = undoStack.back();
	undoStack.pop_back();

	loadSnapshot(snap);
}

void MapEditor::redo()
{
	if (redoStack.isEmpty()) return;

	// 現在をundoへ
	EditorSnapshot current{
		state.grid,
		gimmikManager.getGimmiks(),
		startPlayerPos
	};

	undoStack << current;

	// 進む
	auto snap = redoStack.back();
	redoStack.pop_back();

	loadSnapshot(snap);
}

// 現状のマップが空かどうか
bool MapEditor::canSaveFile() const
{
	if (saveFileName.text == U"")return false;

	int endX = state.mapSize.x;
	int endY = state.mapSize.y;

	for (int y = 0; y < endY; ++y)
	{
		for (int x = 0; x < endX; ++x)
		{
			if (state.grid[y][x])
			{
				return true;
			}
		}
	}

	return false;
}
