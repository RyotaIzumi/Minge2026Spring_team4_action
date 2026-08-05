#include "Item.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	Item::Item(Vec2 startPos, ItemType ItemType) {
		const double side = 32;
		pos = { startPos.x * side, startPos.y * side };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side,side });
		type = ObjectType::Item;

		canPlayerKill = false;
		isPlayerTouching = false;

		depth = 20;
	}

	void Item::update() {
		if (isPlayerTouching) {
			Global::prepareGetItem1 = true;
			alpha -= 0.1;
		}
		if (alpha < 0) isDelete = true;
	}

	void Item::draw() const {
		TextureAsset(U"item1").draw(pos,ColorF(1.0,alpha));
	}

	void Item::onCollision(GameObject& other) {
	}
}
