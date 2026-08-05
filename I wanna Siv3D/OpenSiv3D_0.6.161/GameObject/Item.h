#pragma once
#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Sprite/SpriteSystem.h"
#include "../Global.h"
#include "GameObject.h"

	namespace Iwanna {
	enum class ItemType {
		Heart,
		Warp
	};

	class Item : public GameObject {
	private:
		ItemType itemType;
	public:
		bool isPlayerTouching = false;

		Item(Vec2 startPos, ItemType signtype);
		ItemType getItemType() const;

		void update() override;
		void draw() const override;
		void onCollision(GameObject& other) override;
	};
}
