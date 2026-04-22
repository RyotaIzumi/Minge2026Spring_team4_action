#include "Sign.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	Sign::Sign(Vec2 startPos, SignType signType) {
		const double side = 32;
		pos = { startPos.x * side, startPos.y * side };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side,side });
		type = ObjectType::Sign;

		canPlayerKill = false;
		isPlayerTouching = false;

		depth = 20;
	}

	void Sign::update() {
		if (signAlpha < 1.0 && isPlayerTouching) signAlpha += 0.04;
		else if (signAlpha >= 0.0)signAlpha -= 0.04;
	}

	void Sign::draw() const {
		FontAsset(U"PlayerMessage")(U"Press Q to escape").drawAt(pos, ColorF(Palette::Whitesmoke, signAlpha));
	}
}
