#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Sprite/SpriteSystem.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	class SavePoint : public GameObject {
	private:
		int32 side = 32;
		bool isSaving = false;
		Timer saveIntervalTimer{ SecondsF{ 1.0 }, StartImmediately::No };
	public:
		SavePoint(Vec2 startPos);

		void update() override;
		void draw() const override;
		void saved();
		std::function<void()> onSavedCallback;
		void onCollision(GameObject& other) override;
	};
}
