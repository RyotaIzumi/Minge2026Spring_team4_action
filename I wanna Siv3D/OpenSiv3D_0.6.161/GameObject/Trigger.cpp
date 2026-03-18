#include "Trigger.h"

namespace Iwanna {
	Trigger::Trigger(Vec2 startPos, int32 id, double sizeX, double sizeY, bool checkPrevId) {
		//GameObject.hの値初期化
		pos = { startPos.x, startPos.y };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side * sizeX,side * sizeY });
		type = ObjectType::Trigger;

		checkPrevID = checkPrevId;
		canPlayerKill = false;
		checkOtherCondition = false;
		trapID = id;
	}
	Trigger::Trigger(Vec2 startPos, int32 id, double sizeX, double sizeY, std::function<bool()> checkOtherConditionFunc) {
		//GameObject.hの値初期化
		pos = { startPos.x, startPos.y };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side * sizeX,side * sizeY });
		type = ObjectType::Trigger;

		checkPrevID = false;
		canPlayerKill = false;
		checkOtherCondition = true;
		this->checkOtherConditionFunc = checkOtherConditionFunc;
		trapID = id;
	}
	void Trigger::update() {
	}
	void Trigger::draw() const {
		//hitBox->draw(Palette::Pink);
	}
	void Trigger::onCollision(GameObject& other) {
	}

	// トリガーを作動させる
	void Trigger::triggerActivate() {
		isActivated = true;
	}
	// トリガーが作動しているかどうかを取得
	bool Trigger::getIsActivated() const {
		return isActivated;
	}
	// 直前のトリガーIDを確認するかどうかを取得
	bool Trigger::getCheckPrevID() const {
		return checkPrevID;
	}
	// 他の条件を必要とするかどうかを取得
	bool Trigger::getCheckOtherCondition() const {
		return checkOtherCondition;
	}
	// 罠のIDを取得
	int32 Trigger::getTrapID() const {
		return trapID;
	}
}
