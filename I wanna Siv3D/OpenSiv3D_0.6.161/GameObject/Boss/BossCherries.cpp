#include "BossCherry.h"

namespace Iwanna{
	// ----- 弾幕用りんご ----- //
	BossFallBlueCherry::BossFallBlueCherry(Vec2 startPos, double scale, BossCherryType cType) : BossBarrageCherry(startPos, scale, cType) {

		//GameObject.hの値初期化
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);
		type = ObjectType::Cherry;
		cherryType = CherryType::Barrage;
		cherrySubType = cType;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = false;
		isTrap = false;

		alpha = 1.0;
		startStep = 0;
	}

	void BossFallBlueCherry::barrageUpdate() {
		
		switch (startStep) {
		case 0:
			gravity = 0.2;
			speed = 1;
			direction = 270;
			startStep++;
			break;
		case 1:
			if (pos.y > 700) {
				isDelete = true;
			}
			break;
		}

		setTypeColor();
	}
}
