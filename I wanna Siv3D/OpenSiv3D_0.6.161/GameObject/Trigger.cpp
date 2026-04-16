#include "Trigger.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	Trigger::Trigger(Vec2 startPos, int32 id, double sizeX, double sizeY, bool checkPrevId) {
		//GameObject.hの値初期化
		pos = { startPos.x, startPos.y };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side * sizeX,side * sizeY });
		type = ObjectType::Trigger;

		checkPrevID = checkPrevId;
		canPlayerKill = false;
		triggerType = TriggerType::Normal;
		trapID = id;
	}
	Trigger::Trigger(Vec2 startPos, int32 id, double sizeX, double sizeY, std::function<bool()> checkOtherConditionFunc) {
		//GameObject.hの値初期化
		pos = { startPos.x, startPos.y };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side * sizeX,side * sizeY });
		type = ObjectType::Trigger;

		checkPrevID = false;
		canPlayerKill = false;
		triggerType = TriggerType::OtherCondition;
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
	// 罠のIDを取得
	int32 Trigger::getTrapID() const {
		return trapID;
	}
	// トリガーの種類を取得
	TriggerType Trigger::getTriggerType() const {
		return triggerType;
	}

	// ----- アイテム部屋関連用のトリガー ----- //
	SecretTrigger::SecretTrigger(Vec2 startPos) : Trigger(pos, 0, 1.0, 1.0, false) {
		const double side = 32;
		pos = { startPos.x * side, startPos.y * side };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side,side });
		triggerType = TriggerType::Secret;
	}

	void SecretTrigger::triggerActivate() {
		if (!Global::isSecretTriggerActivated) {
			AudioAsset(Sound::BLOCKCHANGE).playOneShot();
			Global::isSecretTriggerActivated = true;
		}
	}
}
