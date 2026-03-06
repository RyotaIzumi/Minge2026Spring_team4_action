#include "Blood.h"
#include "../Audio/AudioAsset.h"
#include "../GameObject/SavePoint.h"

namespace Iwanna {
	Blood::Blood(Vec2& genePos, double hs) {

		//GameObject.hの値初期化
		pos = genePos;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize);
		type = ObjectType::Blood;
		canPlayerKill = false;
		isDelete = false;
		isOutOfScreen = false;

		hspeed = hs;
		vspeed = 0;
	}

	void Blood::update() {
		checkOutOfScreen();
		// 位置更新
		pos.x += hspeed;
		pos.y += vspeed;

		// 当たり判定位置更新
		hitBox->setPos(pos);
	}

	void Blood::draw() const {
		//Rect(0,0,1,1).drawAt(pos.x, pos.y);
		hitBox->draw(Palette::Red);//判定の可視化(そのまま血に利用)
	}

	void Blood::onCollision(GameObject& other) {
		// ブロック、ミク衝突
		if (other.type == ObjectType::Block) {
			isDelete = true;
		}
	}

	//画面外判定
	void Blood::checkOutOfScreen() {
		const int32 excess = hitBoxSize * 2;//画面端からの余白
		if (pos.x < -1 * excess || pos.x > Global::stageWidth + excess ||
			pos.y < -1 * excess || pos.y > Global::stageHeight + excess) {
			isOutOfScreen = true;
		}
		else {
			isOutOfScreen = false;
		}
	}
}
