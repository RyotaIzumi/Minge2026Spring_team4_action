#include "Scene.h"

namespace Iwanna {
	InGame::InGame(const InitData& data) : IScene(data) {

	}

	void InGame::update() {
		auto& data = getData().game;

		data.updateGame();

		if (Global::inputRestart.down() || Global::isChangeRoom) {
			data.startGame();
			Window::SetTitle(U"I wanna Siv3D (Debug Build)");

			changeScene(SceneType::IN_GAME, 0.0s);
		}

		debug();
	}

	void InGame::debug() const {
		auto& data = getData().game;
		data.debugGame();
	}

	void InGame::draw() const {
		auto& data = getData().game;

		data.drawGame();
	}
}
