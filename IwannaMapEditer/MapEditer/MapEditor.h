#pragma once
#include <Siv3D.hpp>
#include "EditorState.h"
#include "AutoTile.h"
#include "Gimmik/GimmikManager.h"
#include "MapSerializer.h"
#include "History.h"

class MapEditor
{
private:
	EditorState state;

	Array<AutoTile> autoTiles;
	Texture baseTexture{ U"texture/object/none.png" };

	static constexpr int VIEW_WIDTH = 25;
	static constexpr int VIEW_HEIGHT = 19;
	static constexpr Point OFFSET{ 40, 40 };

	GimmikManager gimmikManager;

	//タイル選択用
	Grid<uint32> tileGrid{ Size{5, 9}, 0 };
	Optional<Point> selectedTileCursorIndex = Point{ 0,0 };

	//マップ保存用
	TextEditState saveFileName;
	FilePath mainActionProjectPath = U"../../../I wanna Siv3D/OpenSiv3D_0.6.161/App/MapData/";

	//player位置保存用
	Vec2 startPlayerPos{ 30, 30 };
	TextEditState playerXText;
	TextEditState playerYText;

	// background変更用
	ListBoxState backgroundListBox;
	String currentBackground;
	Array<String> backgroundNames;

	// redo/undo用
	Array<EditorSnapshot> undoStack;
	Array<EditorSnapshot> redoStack;
	static constexpr size_t MAX_HISTORY = 50;

public:
	MapEditor();

	void update();
	void draw();

private:
	void updateInput();
	void updateMapSize();
	void updatePageScroll();
	void updateTilePlacement();
	void updateUndoRedo();

	void drawMap();
	void drawMapSizeUI();
	void drawPlayerUI();
	void drawBackgroundUI();
	void drawPlayerOnMap(int tileSize);
	void drawPageInfo();
	void drawCursor();
	void drawTileSelector(int32 tileSize);

	void saveSnapshot();
	void loadSnapshot(const EditorSnapshot& snapshot);
	void undo();
	void redo();
};
