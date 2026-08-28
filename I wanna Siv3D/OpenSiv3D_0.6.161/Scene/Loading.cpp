#include "Scene.h"
#include "../Sprite/SpriteAsset.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	Loading::Loading(const InitData& init) : IScene(init) {
		loadingTask = Async([this]() { load(); });
	}

	void Loading::load() {
		registerTextures();
		registerTexturesSync();
		loadTexturesSync();
		Sound::registerBGMs();
		Sound::registerSEs();
		Sound::registerAudiosSync();
		Sound::loadAudiosSync();
	}

	void Loading::update() {
		if (loadingTask.isReady()) {
			changeScene(SceneType::START_MENU, 0.25s);
		}
	}

	void Loading::draw() const {
		const int32 w = Scene::Width();
		const int32 h = Scene::Height();
		Rect{ 0, 0, w, h }.draw(Palette::Black);

		const Vec2 loadingTextPos{ w - 170.0, h - 50.0 };
		FontAsset(U"BossHp")(Global::localized(U"ロード中", U"Loading")).drawAt(loadingTextPos, Palette::White);

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
