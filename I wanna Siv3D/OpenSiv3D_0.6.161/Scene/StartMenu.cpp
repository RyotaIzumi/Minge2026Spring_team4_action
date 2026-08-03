#include "Scene.h"

namespace Iwanna {
	StartMenu::StartMenu(const InitData& data) : IScene(data) {

	}

	void StartMenu::update() {
		auto& data = getData().game;

		if (Global::inputStart.down()) {
			data.startNewGame();
			changeScene(SceneType::IN_GAME, 0.0s);
		}
	}

	void StartMenu::draw() const {
		FontAsset(U"Button")(U"Escでメニューを開けます").draw(24, 24, ColorF{ 1.0, 1.0, 1.0 });
		if (Global::getItem1) {
			TextureAsset(U"heart").draw(24, Global::windowHeight - 56);
		}
		FontAsset(U"Big")(U"Press shift to start").drawAt(400,304);
	}
}
