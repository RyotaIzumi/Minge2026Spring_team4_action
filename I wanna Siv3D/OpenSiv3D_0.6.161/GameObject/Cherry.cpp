#include "Cherry.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	Cherry::Cherry() {

		//GameObject.hの値初期化
		pos = Vec2(0, 0);
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize);
		type = ObjectType::Cherry;
		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isTrap = false;

		speed = 0;
		direction = 0;
	}

	void Cherry::update() {
		checkOutOfScreen();

		if (speed != 0) {
			calculateSpeed();

			// 位置更新
			pos.x += hspeed;
			pos.y += vspeed;
		}
		// 当たり判定位置更新
		hitBox->setPos(pos);
	}

	void Cherry::trapUpdate(int32 id) {
	}

	void Cherry::draw() const {
		TextureAsset(U"sprCherryLow").drawAt(pos.x,pos.y-1);
		//hitBox->draw(Palette::Blue);//判定の可視化
	}

	//画面外判定
	void Cherry::checkOutOfScreen() {
		const int32 excess = hitBoxSize * 2;//画面端からの余白
		if (pos.x < -1 * excess || pos.x > Global::stageWidth + excess ||
			pos.y < -1 * excess || pos.y > Global::stageHeight + excess) {
			isOutOfScreen = true;
		}
		else {
			isOutOfScreen = false;
		}
	}

	void Cherry::onCollision(GameObject& other) {
	}

	CherryTrap::CherryTrap(Vec2 startPos, int32 id, double dir, double spd) : trapID(id) {
		pos = startPos;
		direction = dir;
		trapSpeed = spd;
		speed = 0;
		hspeed = 0;
		vspeed = 0;

		isTrap = true;
	}

	void CherryTrap::trapUpdate(int32 id) {
		checkOutOfScreen();
		if (trapID == id) {
			isTrapActived = true;
		}

		if (isTrapActived) {
			speed = trapSpeed;
		}
	}

	//罠のIDを取得
	int32 CherryTrap::getTrapID() const {
		return trapID;
	}
}
