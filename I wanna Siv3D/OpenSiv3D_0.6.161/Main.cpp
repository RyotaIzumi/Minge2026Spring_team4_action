#include <Siv3D.hpp> // Siv3D v0.6.16
#include "Sprite/SpriteAsset.h"
#include "Audio/AudioAsset.h"
#include "Scene/Scene.h"
#include "MainGameSerializer.h"
#include "60FPSwithAutoFrameSkip.h"
#include "Global.h"

using App = SceneManager<Iwanna::SceneType, Iwanna::CommonData>;

void Main()
{
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	Window::SetTitle(U"I wanna continue Siv3D");

	MainGameSerializer mainGameSerializer;
	mainGameSerializer.LoadGameSettings();
	System60::SetDisplaySize(Size{800,608}, Global::isFullscreen);

	//フォントはここで宣言
	FontAsset::Register(U"Big", 60, Typeface::Regular);
	FontAsset::Register(U"BossHp", FontMethod::MSDF, 30, Typeface::Medium);
	FontAsset::Register(U"Button", FontMethod::MSDF, 20, Typeface::Medium);
	FontAsset::Register(U"TitleTrap", FontMethod::MSDF, 10, Typeface::Medium);
	FontAsset::Register(U"PlayerMessage", FontMethod::MSDF, 12, Typeface::Medium);
	FontAsset::Register(U"TitleCard", FontMethod::MSDF, 20, Typeface::Medium);
	FontAsset::Register(U"Achieve", FontMethod::MSDF, 16, Typeface::Medium);

	//カーソル設定
	Image cursorImage{ U"✌"_emoji };
	Cursor::RegisterCustomCursorStyle(U"cursorPiece", cursorImage.scaled(0.5), Point{10,10});
	
	App app;
	app.add<Iwanna::Loading>(Iwanna::SceneType::LOADING);
	app.add<Iwanna::StartMenu>(Iwanna::SceneType::START_MENU);
	app.add<Iwanna::InGame>(Iwanna::SceneType::IN_GAME);
	app.init(Iwanna::SceneType::LOADING, 0s);

	mainGameSerializer.LoadCharactersMoraleValue();
	mainGameSerializer.LoadEndingValue();
	mainGameSerializer.defineGlobalStatuses();
	Global::elapsedPlayTime = 0.0;
	Global::deathCount = 0;

	while (System60::Update()) {
		if (not app.update()) {
			break;
		}
	}
	mainGameSerializer.SaveCharactersMoraleValue();
	mainGameSerializer.SaveGameSettings();
	mainGameSerializer.SaveEndingClearRecord();
	mainGameSerializer.SaveEndingValue();
}
