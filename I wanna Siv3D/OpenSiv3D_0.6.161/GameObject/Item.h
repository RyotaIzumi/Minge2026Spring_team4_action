#pragma once
#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Sprite/SpriteSystem.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	enum class ItemType {
		Heart
	};

	class Item : public GameObject {
	public:
		bool isPlayerTouching = false;

		Item(Vec2 startPos, ItemType signtype);

		void update() override;
		void draw() const override;
		void onCollision(GameObject& other) override;
	};
}
