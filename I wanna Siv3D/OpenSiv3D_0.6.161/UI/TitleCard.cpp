#include "TitleCard.h"
#include "../Audio/AudioAsset.h"

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
			canShowTitleCard = true;
			moveTimer.restart();
			Sound::playOneShot(Sound::TITLECARD);

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
			nowPos.y = startPos.y + cardSize.y - cardSize.y * EaseInSine(moveTimer.progress0_1());
			if (moveTimer.reachedZero()) {
				reset();
			}
			break;
		}
	}

	void TitleCard::reset() {
		nowPos = startPos;
		canShowTitleCard = false;
		moveStep = -1;
	}

	void TitleCard::setNowCameraPos(Vec2 pos) {
		cameraPos = pos;
	}

	//カード表示を開始させる
	void TitleCard::startShowTitleCard(String roomName) {
		if (roomName == U"boss") {
			titleCardText = U"Gardian Gate";
			titleCardTexture = U"sprTitleCard_boss";
		}
		if (roomName == U"ExBoss") {
			titleCardText = U"Treasure House";
			titleCardTexture = U"sprTitleCard_boss";
		}
		if (roomName == U"secret1") {
			titleCardText = U"Secret Item Room";
			titleCardTexture = U"sprTitleCard_secret";
		}
		moveStep = 0;
	}

	void TitleCard::draw() const {

		if(!canShowTitleCard) return;

		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(titleCardTexture).draw(nowPos).drawFrame(1.0,ColorF(Palette::Black));

		// 文字表示
		Vec2 textBasePos = nowPos + Vec2(cardSize.x / 2, cardSize.y - 12);
		FontAsset(U"TitleCard")(titleCardText).drawAt(textBasePos.x - 1, textBasePos.y,ColorF(Palette::Black));
		FontAsset(U"TitleCard")(titleCardText).drawAt(textBasePos.x + 1, textBasePos.y,ColorF(Palette::Black));
		FontAsset(U"TitleCard")(titleCardText).drawAt(textBasePos.x, textBasePos.y - 1,ColorF(Palette::Black));
		FontAsset(U"TitleCard")(titleCardText).drawAt(textBasePos.x, textBasePos.y + 1,ColorF(Palette::Black));

		// 本体（白）
		FontAsset(U"TitleCard")(titleCardText).drawAt(textBasePos.x, textBasePos.y);
	}
}
