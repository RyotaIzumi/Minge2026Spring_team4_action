#include "SavePoint.h"

namespace Iwanna {
	SavePoint::SavePoint(Vec2 startPos) {
		//GameObject.hの値初期化
		pos = { startPos.x * side, startPos.y * side };
		hitBox = std::make_shared<RectHitBox>(Vec2{ pos.x,pos.y + 8 }, SizeF{ side,24 });
		type = ObjectType::SavePoint;
		canPlayerKill = false;
	}
	void SavePoint::update() {
		if(isSaving && saveIntervalTimer.reachedZero()) {
			isSaving = false;
		}
	}
	void SavePoint::draw() const {
		//hitBox->draw(Palette::Gray);
		TextureAsset(U"sprSave")(isSaving ? side : 0, 0, side, side).draw(pos);
	}
	// セーブされたときの処理
	void SavePoint::saved() {
		if (!isSaving) {
			isSaving = true;
			saveIntervalTimer.restart();

			if (onSavedCallback) {
				onSavedCallback();
			}
		}
	}
	
	void SavePoint::onCollision(GameObject& other) {
	}

}
