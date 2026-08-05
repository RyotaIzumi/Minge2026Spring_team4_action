#include "Achieve.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	Achieve::Achieve() {
		startPos = Vec2{ 0,Global::windowHeight };
		nowPos = startPos;

		achieveFrameTexture = U"achieveFrame";
		moveStep = -1;
	}

	void Achieve::update() {
		switch (moveStep) {
		case 0://初期化
			nowPos = startPos;
			canShowAchieve = true;
			moveTimer.restart();
			Sound::playOneShot(Sound::ACHIEVE);

			moveStep++;
			break;
		case 1://画面内遷移
			nowPos.y = startPos.y - cardSize.y * EaseOutSine(moveTimer.progress0_1());
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
			nowPos.y = startPos.y - cardSize.y + cardSize.y * EaseInSine(moveTimer.progress0_1());
			if (moveTimer.reachedZero()) {
				reset();
			}
			break;
		}
	}

	void Achieve::reset() {
		nowPos = startPos;
		canShowAchieve = false;
		moveStep = -1;
	}

	void Achieve::setNowCameraPos(Vec2 pos) {
		cameraPos = pos;
	}

	//カード表示を開始させる
	void Achieve::startShowAchieve(AchieveType type) {
		
		switch (type) {
		case AchieveType::ItemGet_Heart:
			achieveText = U"針を銃で壊せるように！\nまた、銃の攻撃力も上昇！";
			achieveIconTexture = U"item1";
			break;
		}
		moveStep = 0;
	}

	void Achieve::draw() const {

		if (!canShowAchieve) return;

		Vec2 iconOffset = Vec2{ 16,16 };
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(achieveFrameTexture).draw(nowPos);
		TextureAsset(achieveIconTexture).draw(nowPos + iconOffset).drawFrame();

		// 文字表示
		Vec2 textBasePos = nowPos + Vec2(cardSize.x / 2 + 32, cardSize.y / 2);
		FontAsset(U"Achieve")(achieveText).drawAt(textBasePos.x - 1, textBasePos.y, ColorF(Palette::Black));
		FontAsset(U"Achieve")(achieveText).drawAt(textBasePos.x + 1, textBasePos.y, ColorF(Palette::Black));
		FontAsset(U"Achieve")(achieveText).drawAt(textBasePos.x, textBasePos.y - 1, ColorF(Palette::Black));
		FontAsset(U"Achieve")(achieveText).drawAt(textBasePos.x, textBasePos.y + 1, ColorF(Palette::Black));

		// 本体（白）
		FontAsset(U"Achieve")(achieveText).drawAt(textBasePos.x, textBasePos.y);
	}
}
