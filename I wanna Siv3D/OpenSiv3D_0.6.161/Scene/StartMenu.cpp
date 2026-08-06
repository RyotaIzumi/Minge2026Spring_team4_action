#include "Scene.h"

#include "../Audio/AudioAsset.h"

namespace Iwanna {
	namespace {
		void drawItemIconWithBlockedMark(const String& textureName, const Vec2& pos) {
			TextureAsset(textureName).draw(pos);
			if (Global::isItemEffectBlockedRoute()) {
				TextureAsset(U"batu").resized(32, 32).draw(pos);
			}
		}
	}

	StartMenu::StartMenu(const InitData& data) : IScene(data) {

	}

	void StartMenu::update() {
		auto& data = getData().game;

		if (Global::inputStart.down()) {
			if (Global::isNoMoraleEndingRoute()) {
				Sound::playOneShot(Sound::ERROR);
				System::MessageBoxOK(U"エラーコード : 0xNOMORALE\nやる気が見つかりません。");
				System::Exit();
				return;
			}

			data.startNewGame();
			changeScene(SceneType::IN_GAME, 0.0s);
		}
	}

	void StartMenu::draw() const {
		FontAsset(U"Button")(U"Escでメニュー").draw(24, 24, ColorF{ 1.0, 1.0, 1.0 });
		if (Global::canUseItem2Effect()) {
			FontAsset(U"Button")(U"Xでアクション切り替え").draw(24, 50, ColorF{ 1.0, 1.0, 1.0 });
		}
		if (Global::getItem1) {
			drawItemIconWithBlockedMark(U"item1", Vec2{ 24, Global::windowHeight - 56 });
		}
		if (Global::getItem2) {
			drawItemIconWithBlockedMark(U"item2", Vec2{ 64, Global::windowHeight - 56 });
		}
		FontAsset(U"Big")(U"Press shift to start").drawAt(400,304);
	}
}
