#pragma once
#include <Siv3D.hpp>

Optional<Point> GetCursorIndexSafe(
	const Size& viewSize,
	int32 tileSize,
	const Point& offset,
	const Size& mapSize,
	double scrollX,
	double scrollY);
