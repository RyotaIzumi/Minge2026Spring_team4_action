#include "GimmikManager.h"

GimmikManager::GimmikManager()
{
	names = {
		U"罠針_上", U"罠針_左", U"罠針_下", U"罠針_右",
		U"罠トリガー", U"前トリガー", U"罠りんご", U"罠ブロック",
		U"ワープ"
	};

	const FilePath path = U"texture/gimmik/";

	textures = {
		Texture{ path + U"spikeTrapUp.png" },
		Texture{ path + U"spikeTrapLeft.png" },
		Texture{ path + U"spikeTrapDown.png" },
		Texture{ path + U"spikeTrapRight.png" },
		Texture{ path + U"trapTrigger.png" },
		Texture{ path + U"trapTriggerPrev.png" },
		Texture{ path + U"trapCherry.png" },
		Texture{ path + U"trapBlock.png" },
		Texture{ path + U"sprWarp.png" }
	};

	listBox.items = names;
}

void GimmikManager::update(const Optional<Point>& cursorIndex, int tileSize)
{
	// 配置
	if (cursorIndex && MouseL.down())
	{
		placeGimmik(*cursorIndex, tileSize);
	}
}

void GimmikManager::placeGimmik(const Point& index, int tileSize)
{
	if (!listBox.selectedItemIndex) return;

	Gimmik g;
	g.name = names[*listBox.selectedItemIndex];
	g.pos = Vec2{ index.x * tileSize, index.y * tileSize };

	// 初期値設定（元コード踏襲）
	if (g.name == U"罠トリガー" || g.name == U"前トリガー")
	{
		g.value1 = 0;
		g.value2 = 1.0;
		g.value3 = 1.0;
	}

	gimmiks << g;
	placedListBox.items << g.name;
}

void GimmikManager::draw(int tileSize, double scrollX, double scrollY)
{
	drawGimmiks(tileSize, scrollX, scrollY);
	drawUI();
}

void GimmikManager::drawGimmiks(int tileSize, double scrollX, double scrollY)
{
	int count = 0;

	for (const auto& g : gimmiks)
	{
		// Array<String> に indexOf が無いので自前で検索する
		int idx = -1;
		for (size_t i = 0; i < names.size(); ++i)
		{
			if (names[i] == g.name)
			{
				idx = static_cast<int>(i);
				break;
			}
		}

		if (idx == -1) continue;

		Vec2 pos{
			g.pos.x - scrollX * tileSize + 40,
			g.pos.y - scrollY * tileSize + 40
		};

		// ギミックによって描画方法を変える
		if (g.name == U"罠トリガー" || g.name == U"前トリガー")
			textures[idx].scaled({ g.value2,g.value3 }).draw(pos).drawFrame(1.0, (placedListBox.selectedItemIndex && count == *placedListBox.selectedItemIndex) ? ColorF(1, 0, 0) : ColorF(0, 0));
		else if(g.name == U"罠りんご")
			textures[idx].drawAt(pos).drawFrame(1.0, (placedListBox.selectedItemIndex && count == *placedListBox.selectedItemIndex) ? ColorF(1, 0, 0) : ColorF(0, 0));
		else
			textures[idx].draw(pos).drawFrame(1.0, (placedListBox.selectedItemIndex && count == *placedListBox.selectedItemIndex)? ColorF(1, 0, 0) : ColorF(0, 0));

		count++;
	}
}

void GimmikManager::drawUI()
{
	const Vec2 base{ 1100, 350 };

	// === リスト ===
	SimpleGUI::ListBox(listBox, Vec2{ 900, 140 }, 120, 156);
	SimpleGUI::ListBox(placedListBox, Vec2{ 900, 350 }, 120, 300);

	if (!placedListBox.selectedItemIndex) return;

	size_t idx = *placedListBox.selectedItemIndex;

	// === 削除 ===
	if (SimpleGUI::Button(U"Delete", Vec2{ base.x, base.y }))
	{
		gimmiks.remove_at(idx);
		placedListBox.items.remove_at(idx);
		return;
	}

	// === 初期化 ===
	if (prevIndex != idx)
	{
		xText.text = Format(gimmiks[idx].pos.x);
		yText.text = Format(gimmiks[idx].pos.y);
		value1Text.text = Format(gimmiks[idx].value1);
		value2Text.text = Format(gimmiks[idx].value2);
		value3Text.text = Format(gimmiks[idx].value3);
		prevIndex = idx;
	}

	int32 valuePosAddHeight = 40;
	int32 value1AddHeight = 80;
	int32 value2AddHeight = 120;
	int32 value3AddHeight = 160;

	FontAsset(U"Font")(U"x").draw(base.x, base.y + valuePosAddHeight);
	FontAsset(U"Font")(U"y").draw(base.x + 85, base.y + valuePosAddHeight);

	// === ラベル === (gimmikによって変更)
	if (gimmiks[idx].name == U"罠針_上" || gimmiks[idx].name == U"罠針_下" || gimmiks[idx].name == U"罠針_右" || gimmiks[idx].name == U"罠針_左" || gimmiks[idx].name == U"罠りんご") {
		FontAsset(U"Font")(U"id : ").draw(base.x, base.y + value1AddHeight);
		FontAsset(U"Font")(U"角度 : ").draw(base.x, base.y + value2AddHeight);
		FontAsset(U"Font")(U"速度 : ").draw(base.x, base.y + value3AddHeight);
	}
	else if (gimmiks[idx].name == U"罠トリガー" || gimmiks[idx].name == U"前トリガー") {
		FontAsset(U"Font")(U"id : ").draw(base.x, base.y + value1AddHeight);
		FontAsset(U"Font")(U"x scale : ").draw(base.x, base.y + value2AddHeight);
		FontAsset(U"Font")(U"y scale : ").draw(base.x, base.y + value3AddHeight);
	}
	else if (gimmiks[idx].name == U"罠ブロック") {
		FontAsset(U"Font")(U"id : ").draw(base.x, base.y + value1AddHeight);
		FontAsset(U"Font")(U"").draw(base.x, base.y + value2AddHeight);
		FontAsset(U"Font")(U"").draw(base.x, base.y + value3AddHeight);
	}
	else if (gimmiks[idx].name == U"ワープ") {
		FontAsset(U"Font")(U"stage ").draw(base.x, base.y + value1AddHeight);
		FontAsset(U"Font")(U"next x").draw(base.x, base.y + value2AddHeight);
		FontAsset(U"Font")(U"next y").draw(base.x, base.y + value3AddHeight);
	}

	// === 入力 ===
	SimpleGUI::TextBox(xText, Vec2{ base.x + 20, base.y + valuePosAddHeight }, 60);
	SimpleGUI::TextBox(yText, Vec2{ base.x + 100, base.y + valuePosAddHeight }, 60);
	SimpleGUI::TextBox(value1Text, Vec2{ base.x + 80, base.y + value1AddHeight }, 80);
	SimpleGUI::TextBox(value2Text, Vec2{ base.x + 80, base.y + value2AddHeight }, 80);
	SimpleGUI::TextBox(value3Text, Vec2{ base.x + 80, base.y + value3AddHeight }, 80);

	// === 反映 ===
	if (xText.textChanged && xText.text != U"")
	{
		gimmiks[idx].pos.x = Parse<double>(xText.text);
	}
	if (yText.textChanged && yText.text != U"")
	{
		gimmiks[idx].pos.y = Parse<double>(yText.text);
	}
	if (value1Text.textChanged && value1Text.text != U"")
	{
		if(gimmiks[idx].name == U"ワープ") gimmiks[idx].valueString = value1Text.text;
		else gimmiks[idx].value1 = Parse<int32>(value1Text.text);
	}
	if (value2Text.textChanged && value2Text.text != U"")
	{
		gimmiks[idx].value2 = Parse<double>(value2Text.text);
	}
	if (value3Text.textChanged && value3Text.text != U"")
	{
		gimmiks[idx].value3 = Parse<double>(value3Text.text);
	}
}
