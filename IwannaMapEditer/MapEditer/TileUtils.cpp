#include "TileUtils.h"

Optional<Point> GetCursorIndexSafe(
	const Size& viewSize,
	int32 tileSize,
	const Point& offset,
	const Size& mapSize,
	double scrollX,
	double scrollY)
{
	const Point cursorPos = Cursor::Pos() - offset;

	if (!InRange(cursorPos.x, 0, viewSize.x * tileSize - 1) ||
		!InRange(cursorPos.y, 0, viewSize.y * tileSize - 1))
	{
		return none;
	}

	Point index = cursorPos / tileSize;
	index.x += static_cast<int32>(scrollX);
	index.y += static_cast<int32>(scrollY);

	if (index.x >= mapSize.x || index.y >= mapSize.y)
	{
		return none;
	}

	return index;
}
