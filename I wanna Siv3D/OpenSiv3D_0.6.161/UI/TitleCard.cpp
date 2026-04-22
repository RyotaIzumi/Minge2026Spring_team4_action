#include "TitleCard.h"

namespace Iwanna {
	TitleCard::TitleCard() {
		startPos = Vec2{Global::windowWidth - cardSize.x,-cardSize.y};
		nowPos = startPos;
		moveStep = -1;
	}

	void TitleCard::update() {
		switch (moveStep) {
		case 0://初期化
			nowPos = startPos;
			moveTimer.restart();

			moveStep++;
			break;
		case 1://画面内遷移
			nowPos.y = startPos.y + cardSize.y * EaseOutSine(moveTimer.progress0_1());
			if (moveTimer.reachedZero()) {
				waitTimer.restart();
				moveStep++;
			}
			break;
		case 2://停止
			if (waitTimer.reachedZero()) {
				moveTimer.restart();
				moveStep++;
			}
			break;
		case 3://画面外遷移
			nowPos.y = startPos.y + cardSize.y - cardSize.y * EaseOutSine(moveTimer.progress0_1());
			if (moveTimer.reachedZero()) {
				nowPos = startPos;
				moveStep = -1;
			}
			break;
		}
	}

	void TitleCard::setNowCameraPos(Vec2 pos) {
		cameraPos = pos;
	}

	//カード表示を開始させる
	void TitleCard::startShowTitleCard(String roomName) {
		if (roomName == U"boss") {
			titleCardText = U"Gardian Cherry";
			titleCardTexture = U"sprTitleCard_boss";
		}
		moveStep = 0;
	}

	void TitleCard::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(titleCardTexture).draw(nowPos);

		// 文字表示
		Vec2 textBasePos = nowPos + Vec2(cardSize.x / 2, cardSize.y - 6);
		FontAsset(U"TitleCard")(titleCardText).drawAt(textBasePos.x - 1, textBasePos.y);
		FontAsset(U"TitleCard")(titleCardText).drawAt(textBasePos.x + 1, textBasePos.y);
		FontAsset(U"TitleCard")(titleCardText).drawAt(textBasePos.x, textBasePos.y - 1);
		FontAsset(U"TitleCard")(titleCardText).drawAt(textBasePos.x, textBasePos.y + 1);

		// 本体（白）
		FontAsset(U"TitleCard")(titleCardText).drawAt(textBasePos.x, textBasePos.y);
	}
}
