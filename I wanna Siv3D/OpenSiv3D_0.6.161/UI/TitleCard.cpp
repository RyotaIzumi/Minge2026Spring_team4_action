#include "TitleCard.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	TitleCard::TitleCard() {
		startPos = Vec2{Global::windowWidth - cardSize.x,-cardSize.y};
		showPos = Vec2{ Global::windowWidth - cardSize.x, 0 };
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
			nowPos = startPos.lerp(showPos, EaseOutSine(moveTimer.progress0_1()));
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
			nowPos = showPos.lerp(startPos, EaseInSine(moveTimer.progress0_1()));
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

	void TitleCard::setEntryByPlayerPosition(Vec2 playerPos, Vec2 viewCenter) {
		cameraPos = viewCenter;

		const Vec2 viewTopLeft = viewCenter - Vec2{ Global::windowWidth / 2.0, Global::windowHeight / 2.0 };
		const Vec2 playerScreenPos = playerPos - viewTopLeft;
		const bool isPlayerLeft = playerScreenPos.x < Global::windowWidth / 2.0;
		const bool isPlayerTop = playerScreenPos.y < Global::windowHeight / 2.0;

		const double cardX = isPlayerLeft
			? viewTopLeft.x + Global::windowWidth - cardSize.x
			: viewTopLeft.x;
		const double hiddenY = isPlayerTop
			? viewTopLeft.y + Global::windowHeight
			: viewTopLeft.y - cardSize.y;
		const double shownY = isPlayerTop
			? viewTopLeft.y + Global::windowHeight - cardSize.y
			: viewTopLeft.y;

		startPos = Vec2{ cardX, hiddenY };
		showPos = Vec2{ cardX, shownY };
		nowPos = startPos;
	}

	//カード表示を開始させる
	void TitleCard::startShowTitleCard(String roomName) {
		if (!Global::isExtraStage(roomName)) {
			startPos = Vec2{ Global::windowWidth - cardSize.x, -cardSize.y };
			showPos = Vec2{ Global::windowWidth - cardSize.x, 0 };
		}

		if (roomName == U"boss") {
			titleCardText = U"Gardian Gate";
			titleCardTexture = U"sprTitleCard_boss";
		}
		if (roomName == U"ExBoss") {
			titleCardText = U"Treasure House";
			titleCardTexture = U"sprTitleCard_exboss";
		}
		if (roomName == U"secret1") {
			titleCardText = U"Secret Item Room";
			titleCardTexture = U"sprTitleCard_secret";
		}
		if (roomName == U"ExMiluArea") {
			titleCardText = U"MILU STAGE";
			titleCardTexture = U"sprTitleCard_milu";
		}
		if (roomName == U"ExMochiArea") {
			titleCardText = U"MOCHI STAGE";
			titleCardTexture = U"sprTitleCard_mochi";
		}
		if (roomName == U"ExGotArea") {
			titleCardText = U"GOT STAGE";
			titleCardTexture = U"sprTitleCard_got";
		}
		if (roomName == U"ExRyutaArea") {
			titleCardText = U"RYUTA STAGE";
			titleCardTexture = U"sprTitleCard_ryuta";
		}
		if (roomName == U"ExRenoArea") {
			titleCardText = U"RENO STAGE";
			titleCardTexture = U"sprTitleCard_reno";
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
