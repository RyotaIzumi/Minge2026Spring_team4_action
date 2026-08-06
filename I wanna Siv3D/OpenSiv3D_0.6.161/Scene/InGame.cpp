#include "Scene.h"

namespace Iwanna {
	namespace {
		String formatPlayTime(double seconds) {
			const int32 totalSeconds = static_cast<int32>(seconds);
			const int32 minutes = totalSeconds / 60;
			const int32 displaySeconds = totalSeconds % 60;
			const int32 centiseconds = static_cast<int32>((seconds - totalSeconds) * 100);

			const String secondText = (displaySeconds < 10 ? U"0" : U"") + Format(displaySeconds);
			const String centisecondText = (centiseconds < 10 ? U"0" : U"") + Format(centiseconds);
			return Format(minutes) + U":" + secondText + U"." + centisecondText;
		}

		String getEndingLetter() {
			return String{ static_cast<char32>(U'A' + Clamp(Global::endingValue, 0, 10)) };
		}

		void drawItemIconWithBlockedMark(const String& textureName, const Vec2& pos) {
			TextureAsset(textureName).draw(pos);
			if (Global::isItemEffectBlockedRoute()) {
				TextureAsset(U"batu").resized(32, 32).draw(pos);
			}
		}

		bool isExtraMenuContext() {
			return Global::isExtraStage(Global::nowRoomName) || Global::nowRoomName == U"ExBoss";
		}
	}

	InGame::InGame(const InitData& data) : IScene(data) {

	}

	void InGame::update() {
		auto& data = getData().game;

		if (Global::isGenerateStageFakeLoading) {
			Global::isGenerateStageFakeLoading = false;
			isGenerateLoadingOpen = true;
			generateLoadingTimer.restart();
		}

		if (isGenerateLoadingOpen) {
			if (generateLoadingTimer.reachedZero()) {
				isGenerateLoadingOpen = false;
			}
			return;
		}

		if (isPauseMenuOpen) {
			data.applySoundVolume();
			if (KeyEscape.down()) {
				isPauseMenuOpen = false;
			}
			else if (KeyQ.down()) {
				System::Exit();
			}
			else if (!isExtraMenuContext() && KeyW.down()) {
				data.resetGameToStartMenu();
				isPauseMenuOpen = false;
				isGenerateLoadingOpen = false;
				changeScene(SceneType::START_MENU, 0.0s);
			}
			return;
		}

		if (KeyEscape.down()) {
			isPauseMenuOpen = true;
			return;
		}

		data.updateGame();

		if (Global::inputRestart.down() || Global::isChangeRoom) {
			if (Global::inputRestart.down()) {
				Global::isRestartRoomReload = true;
			}
			data.startGame();

			if (Global::moraleValue2 > 90) Window::SetTitle(U"TestPlayGame (Debug Build)");
			else Window::SetTitle(U"TestPlayGame");

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
		if (isGenerateLoadingOpen) {
			drawGenerateLoading();
		}
		if (isPauseMenuOpen) {
			drawPauseMenu();
		}
	}

	void InGame::drawPauseMenu() const {
		Rect{ 0, 0, Global::windowWidth, Global::windowHeight }.draw(ColorF{ 0.0, 0.0, 0.0, 0.65 });

		FontAsset(U"Button")(U"Escでゲームに戻る").draw(24, 24, ColorF{ 1.0, 1.0, 1.0 });
		if (Global::canUseExBossItem2Effect()) {
			FontAsset(U"Button")(U"Xキーでワープ").draw(24, 50, ColorF{ 1.0, 1.0, 1.0 });
		}
		else if (Global::canUseItem2Effect()) {
			FontAsset(U"Button")(U"Xでアクション切り替え").draw(24, 50, ColorF{ 1.0, 1.0, 1.0 });
		}

		const String quitText = U"ゲームをやめる : Qキー";
		const bool isExtraMenu = isExtraMenuContext();
		const String restartText = isExtraMenu ? U"(進捗は保存されます)" : U"ゲームを最初からやり直す : Wキー";
		const String cautionText = U"(テストプレイの進捗は失われます！)";
		const String endingText = U"到達するエンディング : " + getEndingLetter();
		const String deathText = U"Death " + Format(Global::deathCount);
		const String timeText = U"Play Time " + formatPlayTime(Global::elapsedPlayTime);

		FontAsset(U"BossHp")(quitText).drawAt(400, 172, ColorF{ 1.0, 1.0, 1.0 });
		FontAsset(U"BossHp")(restartText).drawAt(400, 220, ColorF{ 1.0, 1.0, 1.0 });
		if (!isExtraMenu) {
			FontAsset(U"Button")(cautionText).drawAt(400, 268, ColorF{ 1.0, 0.15, 0.15 });
		}
		FontAsset(U"BossHp")(endingText).drawAt(400, 318, ColorF{ 1.0, 1.0, 1.0 });
		FontAsset(U"BossHp")(deathText).drawAt(400, 394, ColorF{ 1.0, 1.0, 1.0 });
		FontAsset(U"BossHp")(timeText).drawAt(400, 438, ColorF{ 1.0, 1.0, 1.0 });
		if (Global::canUseMenuSettings()) {
			const bool previousFullscreen = Global::isFullscreen;
			SimpleGUI::CheckBox(Global::isFullscreen, U"フルスクリーン", Vec2{ 420, 524 }, 260);
			if (Global::isFullscreen != previousFullscreen) {
				Global::applyWindowMode();
			}
			SimpleGUI::Slider(U"サウンド", Global::soundVolume, 0.0, 1.0, Vec2{ 420, 560 }, 110, 260);
			Global::soundVolume = Clamp(Global::soundVolume, 0.0, 1.0);
		}

		if (Global::getItem1) {
			drawItemIconWithBlockedMark(U"item1", Vec2{ 24, Global::windowHeight - 56 });
		}
		if (Global::getItem2) {
			drawItemIconWithBlockedMark(U"item2", Vec2{ 64, Global::windowHeight - 56 });
		}
	}

	void InGame::drawGenerateLoading() const {
		const int32 w = Scene::Width();
		const int32 h = Scene::Height();
		Rect{ 0, 0, w, h }.draw(ColorF{ 0.0, 0.0, 0.0, 0.72 });

		const Vec2 loadingTextPos{ w - 210.0, h - 50.0 };
		FontAsset(U"BossHp")(U"ステージ生成中").drawAt(loadingTextPos, Palette::White);

		const Vec2 spinnerCenter{ w - 60.0, h - 48.0 };
		const double rotation = Scene::Time() * 180.0;
		const int32 appleCount = 10;
		const double radius = 28.0;
		static const Texture appleTexture{ U"\U0001F34E"_emoji };

		for (int32 i = 0; i < appleCount; ++i) {
			const double t = static_cast<double>(i) / (appleCount - 1);
			const double angle = 55.0 + 250.0 * t + rotation;
			const Vec2 applePos = spinnerCenter + Circular{ radius, Math::ToRadians(angle) };
			const double appleScale = 0.18 + 0.12 * t;
			const double alpha = 0.35 + 0.65 * t;

			appleTexture
				.scaled(appleScale)
				.drawAt(applePos, ColorF{ 1.0, alpha });
		}
	}
}
