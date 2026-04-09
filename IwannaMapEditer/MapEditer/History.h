#pragma once
#include <Siv3D.hpp>
#include "Gimmik/Gimmik.h"

struct EditorSnapshot
{
	Grid<uint32> grid;
	Array<Gimmik> gimmiks;
	Vec2 playerPos;
};
