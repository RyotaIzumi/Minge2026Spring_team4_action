# include <Siv3D.hpp>
#include "MapEditor.h"

void Main()
{
	Window::Resize(1280, 720);

	//フォント宣言
	FontAsset::Register(U"Font", 20, Typeface::Regular);

	MapEditor editor;

	while (System::Update())
	{
		editor.update();
		editor.draw();
	}
}
