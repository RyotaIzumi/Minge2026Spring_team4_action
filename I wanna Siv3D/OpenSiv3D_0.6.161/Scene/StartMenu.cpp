#include "Scene.h"

#include "../Audio/AudioAsset.h"

namespace Iwanna {
	namespace {
		String makeReachableEndingText(const Array<String>& endings) {
			String text = U"到達可能Ending (";
			for (size_t i = 0; i < endings.size(); ++i) {
				if (i != 0) {
					text += U",";
				}
				text += endings[i];
			}
			text += U")";
			return text;
		}

		String makeReachableEndingTextWithF(Array<String> endings) {
			if (Global::canReachEndingFRoute() && !endings.includes(U"F")) {
				endings << U"F";
			}
			return makeReachableEndingText(endings);
		}

		String getReachableEndingText() {
			if (Global::isNoMoraleEndingRoute()) {
				return makeReachableEndingText({ U"J" });
			}
			if (Global::isEndingKRoute()) {
				return makeReachableEndingTextWithF({ U"K", U"E" });
			}
			if (Global::isEndingGRoute()) {
				return makeReachableEndingTextWithF({ U"G", U"E", U"I" });
			}
			if (Global::isEndingDRoute()) {
				return makeReachableEndingTextWithF({ U"D" });
			}
			if (Global::isEndingBRoute()) {
				return makeReachableEndingTextWithF({ U"B", U"E", U"F", U"I" });
			}
			if (Global::isEndingCRoute()) {
				return makeReachableEndingTextWithF({ U"C", U"E", U"I" });
			}
			if (Global::isEndingHRoute()) {
				return makeReachableEndingTextWithF({ U"H", U"E", U"I" });
			}
			return makeReachableEndingTextWithF({ U"A", U"E", U"I" });
		}

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
		const String reachableEndingText = getReachableEndingText();
		FontAsset(U"Button")(reachableEndingText).drawAt(402, 562, ColorF{ 0.0, 0.0, 0.0, 0.75 });
		FontAsset(U"Button")(reachableEndingText).drawAt(400, 560, ColorF{ 1.0, 1.0, 1.0 });
	}
}
