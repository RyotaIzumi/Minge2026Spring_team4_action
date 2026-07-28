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

		drawOffset = Vec2{ 16, -30 };

		switch (signType) {
		case SignType::Tutorial_Move: signText = U"矢印キー : 移動"; break;
		case SignType::Tutorial_Jump: signText = U"Shiftキー : ジャンプ"; break;
		case SignType::Tutorial_DoubleJump: signText = U"ジャンプは最大2回可能"; break;
		case SignType::Tutorial_AdjustJump: signText = U"Shiftキーを押す長さで、\nジャンプ力の調整可"; break;
		case SignType::Tutorial_Shoot: signText = U"Zキー : ショット"; break;
		case SignType::Tutorial_Save: signText = U"ショットをセーブに当てると、\nリトライ位置を更新できます"; break;
		case SignType::Tutorial_Spike: signText = U"針やりんごにはあたっちゃダメ！"; break;
		case SignType::Tutorial_Water: signText = U"水に触れるとジャンプが復活するぞ！"; break;
		case SignType::Tutorial_Item: signText = U"道中では、秘密の入口が現れることがある。\n苦難の道ではあるが、乗り越えると強力なアイテムを入手できるぞ！"; break;
		}

		depth = 20;
	}

	void Sign::update() {
		if (isPlayerTouching) {
			signAlpha += 0.1;
		}
		else {
			signAlpha -= 0.1;
		}

		if (signAlpha > 1.0) signAlpha = 1.0;
		if (signAlpha < 0.0) signAlpha = 0.0;

		isPlayerTouching = false;
	}

	void Sign::draw() const {
		Vec2 textBasePos = pos + drawOffset;
		FontAsset(U"TitleCard")(signText).drawAt(textBasePos.x - 1, textBasePos.y, ColorF(Palette::Black, signAlpha));
		FontAsset(U"TitleCard")(signText).drawAt(textBasePos.x + 1, textBasePos.y, ColorF(Palette::Black, signAlpha));
		FontAsset(U"TitleCard")(signText).drawAt(textBasePos.x, textBasePos.y - 1, ColorF(Palette::Black, signAlpha));
		FontAsset(U"TitleCard")(signText).drawAt(textBasePos.x, textBasePos.y + 1, ColorF(Palette::Black, signAlpha));

		// 本体（白）
		FontAsset(U"TitleCard")(signText).drawAt(textBasePos.x, textBasePos.y, ColorF(Palette::Whitesmoke, signAlpha));

		TextureAsset(U"sprSign").draw(pos);
	}

	void Sign::onCollision(GameObject& other) {
	}
}
