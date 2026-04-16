#include "MapSerializer.h"

void MapSerializer::SaveCSV(const Grid<uint32>& grid, const FilePath& path)
{
	CSV csv;

	for (int y = 0; y < grid.height(); ++y)
	{
		for (int x = 0; x < grid.width(); ++x)
		{
			csv.write(grid[y][x]);
		}
		csv.newLine();
	}

	csv.save(path);
	Print << U"csvファイル : " + path + U"を保存しました!";
}

void MapSerializer::LoadCSV(Grid<uint32>& grid, const FilePath& path)
{
	if (!FileSystem::Exists(path))return;

	CSV csv(path);

	for (int y = 0; y < Min(grid.height(), csv.rows()); ++y)
	{
		for (int x = 0; x < Min(grid.width(), csv.columns(y)); ++x)
		{
			grid[y][x] = Parse<int32>(csv[y][x]);
		}
	}

	Print << U"csvファイル : " + path + U"を読み込みました!";
}

void MapSerializer::SaveJSON(
	const Vec2& playerPos,
	const String& backgroundName,
	const Array<Gimmik>& gimmiks,
	const FilePath& path)
{
	JSON json;
	JSON root;

	// プレイヤー
	root[U"startPlayerPos"].push_back(playerPos.x);
	root[U"startPlayerPos"].push_back(playerPos.y);

	// 背景
	root[U"background"] = backgroundName;

	// ギミック
	for (const auto& g : gimmiks)
	{
		JSON obj;

		obj[U"gimmikName"] = g.name;
		obj[U"gimmikPos"].push_back(g.pos.x);
		obj[U"gimmikPos"].push_back(g.pos.y);

		if (g.name == U"ワープ" || g.name == U"特殊ワープ") {
			obj[U"value1"] = g.valueString;
		}
		else if (g.name == U"罠ブロック") {
			obj[U"value1"] = g.value1;
		}
		else if (g.name == U"昇降針") {
			obj[U"value1"] = g.value1;
			obj[U"value2"] = g.value2;
		}
		else if (g.name == U"移動針_上" || g.name == U"移動針_左" || g.name == U"移動針_下" || g.name == U"移動針_右") {
			obj[U"value1"] = g.value1;
			obj[U"value2"] = g.value2;
			obj[U"value3"] = g.value3;
			obj[U"value4"] = g.value4;
		}
		else {
			obj[U"value1"] = g.value1;
			obj[U"value2"] = g.value2;
			obj[U"value3"] = g.value3;
		}

		root[U"Gimmiks"].push_back(obj);
	}

	json.push_back(root);
	json.save(path);

	Print << U"jsonファイル : " + path + U"を保存しました!";
}

void MapSerializer::LoadJSON(
	Vec2& playerPos,
	String& backgroundName,
	Array<Gimmik>& gimmiks,
	const FilePath& path)
{
	if (!FileSystem::Exists(path)) {
		Print << U"jsonファイル : " + path + U"が見つかりません!";
		return;
	}

	JSON json = JSON::Load(path);
	if (!json.isArray() || json.isEmpty()) return;

	const JSON& root = json[0];

	// プレイヤー
	if (root.contains(U"startPlayerPos"))
	{
		playerPos.x = root[U"startPlayerPos"][0].get<double>();
		playerPos.y = root[U"startPlayerPos"][1].get<double>();
	}

	// 背景
	if (root.contains(U"background"))
	{
		backgroundName = root[U"background"].getString();
	}

	// ギミック
	gimmiks.clear();

	if (root.contains(U"Gimmiks"))
	{
		for (const auto& g : root[U"Gimmiks"].arrayView())
		{
			Gimmik obj;

			obj.name = g[U"gimmikName"].getString();
			obj.pos.x = g[U"gimmikPos"][0].get<double>();
			obj.pos.y = g[U"gimmikPos"][1].get<double>();

			if (obj.name == U"ワープ" || obj.name == U"特殊ワープ") {
				obj.valueString = g[U"value1"].get<String>();
				obj.valueNum = 1;
			}
			else if (obj.name == U"罠ブロック") {
				obj.value1 = g[U"value1"].get<int32>();
				obj.valueNum = 1;
			}
			else if (obj.name == U"昇降針") {
				obj.value1 = g[U"value1"].get<int32>();
				obj.value2 = g[U"value2"].get<double>();
				obj.valueNum = 2;
			}
			else if (obj.name == U"移動針_上" || obj.name == U"移動針_左" || obj.name == U"移動針_下" || obj.name == U"移動針_右") {
				obj.value1 = g[U"value1"].get<int32>();
				obj.value2 = g[U"value2"].get<double>();
				obj.value3 = g[U"value3"].get<double>();
				obj.value4 = g[U"value4"].get<double>();
				obj.valueNum = 4;
			}
			else  {
				obj.value1 = g[U"value1"].get<int32>();
				obj.value2 = g[U"value2"].get<double>();
				obj.value3 = g[U"value3"].get<double>();
			}

			gimmiks << obj;
		}
	}

	Print << U"jsonファイル : " + path + U"を読み込みました!";
}
