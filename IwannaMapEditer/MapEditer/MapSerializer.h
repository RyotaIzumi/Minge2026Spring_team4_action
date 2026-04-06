#pragma once
#include <Siv3D.hpp>
#include "Gimmik/Gimmik.h"

class MapSerializer
{
public:
	static void SaveCSV(const Grid<uint32>& grid, const FilePath& path);
	static void LoadCSV(Grid<uint32>& grid, const FilePath& path);

	static void SaveJSON(
		const Vec2& playerPos,
		const Array<Gimmik>& gimmiks,
		const FilePath& path);

	static void LoadJSON(
		Vec2& playerPos,
		Array<Gimmik>& gimmiks,
		const FilePath& path);
};
