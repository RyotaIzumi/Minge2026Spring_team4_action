#pragma once
#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Sprite/SpriteSystem.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	enum class SignType {
		Tutorial_Jump,
	};

	class Sign : public GameObject {
	private:
		String textureName = U"sprBlock";
		bool isPlayerTouching = true;
		double signAlpha = 0.0;
		Rect signRect{0,0,320,160};
	public:
		Sign(Vec2 startPos, SignType signtype);

		void update() override;
		void draw() const override;
	};
}
