#pragma once
#include <Siv3D.hpp>
#include "Gimmik.h"

class GimmikManager
{
private:
	Array<Gimmik> gimmiks;

	Array<String> names;
	Array<int32> valueNums;
	Array<Texture> textures;

	ListBoxState listBox;
	ListBoxState placedListBox;

	// 値編集用
	TextEditState xText,yText;

	TextEditState value1Text;
	TextEditState value2Text;
	TextEditState value3Text;
	TextEditState value4Text;

	Optional<size_t> prevIndex = none;

public:
	GimmikManager();

	void update(const Optional<Point>& cursorIndex, int tileSize);
	void draw(int tileSize, double scrollX, double scrollY);

	/// @brief ギミックの配列を返します。
	Array<Gimmik>& getGimmiks() { return gimmiks; }
	/// @brief ギミックの配列を設定します。
	void setGimmiks(const Array<Gimmik>& newGimmiks)
	{
		gimmiks = newGimmiks;
		placedListBox.items.clear();
		for (const auto& g : gimmiks)
		{
			placedListBox.items << g.name;
		}
	}

private:
	void placeGimmik(const Point& index, int tileSize);
	void drawGimmiks(int tileSize, double scrollX, double scrollY);
	void drawUI();

	template <class T>
	bool checkTextInput(const String& input);
};
