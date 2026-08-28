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
		case SignType::Tutorial_Move: signText = Global::localized(U"矢印キー : 移動", U"Arrow keys : Move"); break;
		case SignType::Tutorial_Jump: signText = Global::localized(U"Shiftキー : ジャンプ", U"Shift : Jump"); break;
		case SignType::Tutorial_DoubleJump: signText = Global::localized(U"ジャンプは最大2回可能", U"You can jump up to twice"); break;
		case SignType::Tutorial_AdjustJump: signText = Global::localized(U"Shiftキーを押す長さで、\nジャンプ力の調整可", U"Hold Shift longer\nto jump higher"); break;
		case SignType::Tutorial_Shoot: signText = Global::localized(U"Zキー : ショット", U"Z : Shoot"); break;
		case SignType::Tutorial_Save: signText = Global::localized(U"ショットをセーブに当てると、\nリトライ位置を更新できます", U"Shoot a save point\nto update your retry position"); break;
		case SignType::Tutorial_Spike: signText = Global::localized(U"針やりんごにはあたっちゃダメ！", U"Do not touch spikes or apples!"); break;
		case SignType::Tutorial_Water: signText = Global::localized(U"水に触れるとジャンプが復活するぞ！", U"Touch water to restore your jump!"); break;
		case SignType::Tutorial_Item: signText = Global::localized(U"道中では、秘密の入口が現れることがある。\n苦難の道ではあるが、乗り越えると強力なアイテムを入手できるぞ！", U"Secret entrances may appear on your journey.\nSurvive the challenge to gain a powerful item!"); break;
		case SignType::Tutorial_Muteki: signText = Global::localized(U"ctrlで無敵になれるぞ\nただし、これを使えば敗北者だ...", U"Ctrl makes you invincible.\nUse it, and you are a loser..."); break;
		case SignType::Extra_PracticeMuteki: signText = Global::localized(U"\nctrlで無敵", U"\nCtrl : Invincible"); break;
		case SignType::Extra_Warp: signText = Global::localized(U"Xキーでワープ可能\nここではクールタイムがあるぞ", U"X lets you warp here.\nIt has a cooldown."); break;
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

		const String textureName = (Global::mainTextureNumber == 0) ? U"sprSign_low" : U"sprSign_normal";
		TextureAsset(textureName).draw(pos);
	}

	void Sign::onCollision(GameObject& other) {
	}
}
