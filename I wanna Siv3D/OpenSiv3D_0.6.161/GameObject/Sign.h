#pragma once
#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Sprite/SpriteSystem.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	enum class SignType {
		Tutorial_Move,
		Tutorial_Jump,
		Tutorial_DoubleJump,
		Tutorial_AdjustJump,
		Tutorial_Shoot,
		Tutorial_Save,
		Tutorial_Spike,
		Tutorial_Water,
		Tutorial_Item,
		Tutorial_Muteki,
	};

	class Sign : public GameObject {
	private:
		String textureName = U"";
		String signText;
		double signAlpha = 0.0;

		Vec2 drawOffset{16, -20};
		Rect signRect{0,0,320,160};
	public:
		bool isPlayerTouching = false;

		Sign(Vec2 startPos, SignType signtype);

		void update() override;
		void draw() const override;
		void onCollision(GameObject& other) override;
	};
}
