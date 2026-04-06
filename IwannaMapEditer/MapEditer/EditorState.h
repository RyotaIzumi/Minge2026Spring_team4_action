#pragma once
#include <Siv3D.hpp>

struct AutoTileConnectivity
{
	bool connected[8] = { true, true, true, true, true, true, true, true };
};

struct EditorState
{
	Size mapSize{ 25, 19 };
	Size gridSize{ 25, 19 };

	Grid<uint32> grid{ gridSize, 0 };
	Grid<AutoTileConnectivity> connectivity{ gridSize };

	TextEditState mapWidthText;
	TextEditState mapHeightText;

	double scrollX = 0;
	double scrollY = 0;

	int32 selectedTileId = 1;
	Optional<Point> cursorIndex;

	size_t settingMode = 0;
};
