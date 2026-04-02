#include "BossCherry.h"

namespace Iwanna{
	// ----- 弾幕用青りんご ----- //
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

	// ----- 弾幕用黄りんご ----- //
	BossYellowStarCherry::BossYellowStarCherry(Vec2 startPos, double scale, BossCherryType cType) : BossBarrageCherry(startPos, scale, cType) {

		//GameObject.hの値初期化
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);
		type = ObjectType::Cherry;
		cherryType = CherryType::Barrage;
		cherrySubType = cType;

		canPlayerKill = false;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = true;
		isTrap = false;

		alpha = 1.0;
		startStep = 0;
		centerPos = startPos;
	}

	void BossYellowStarCherry::barrageUpdate() {

		switch (startStep) {
		case 0:
			pos = centerPos;
			canPlayerKill = true;
			startStep++;
			break;
		case 1:

			break;
		}

		setTypeColor();
	}

	// ----- 弾幕用緑りんご ----- //
	BossGreenWaveCherry::BossGreenWaveCherry(Vec2 startPos, double scale, BossCherryType cType) : BossBarrageCherry(startPos, scale, cType) {

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
		speed = 0;

		gravity = 0.4;
	}

	void BossGreenWaveCherry::barrageUpdate() {

		switch (startStep) {
		case 0:
			if (waveStopwatch.sF() > activeTimer) {
				direction = 90;
				speed = 10;
				startStep++;
			}
			break;
		case 1:
			
			break;
		}

		if (pos.y > 1000) {
			isDelete = true;
		}

		setTypeColor();
	}

	void BossGreenWaveCherry::setActiveTimer(double time) {
		activeTimer = time;
	}

	// ----- 弾幕用オレンジりんご ----- //
	BossOrangeStopCherry::BossOrangeStopCherry(Vec2 startPos, double scale, BossCherryType cType) : BossBarrageCherry(startPos, scale, cType) {

		//GameObject.hの値初期化
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
		speed = 0;

		gravity = 0.4;
	}

	void BossOrangeStopCherry::barrageUpdate() {

		switch (startStep) {
		case 0:
			pos.x = startPos.x + moveRangeX * EaseOutQuad(moveTimer.progress0_1());
			pos.y = startPos.y + moveRangeY * EaseOutQuad(moveTimer.progress0_1());
			if (moveTimer.reachedZero()) {
				startPos = pos;
				moveTimer.restart();
				startStep++;
			}
			break;
		case 1:
			pos.x = startPos.x - moveRangeX * EaseInQuad(moveTimer.progress0_1());
			pos.y = startPos.y - moveRangeY * EaseInQuad(moveTimer.progress0_1());
			if (moveTimer.reachedZero()) {
				startPos = pos;
				moveTimer.restart();
				startStep++;
			}
			break;
		case 2:
			isDelete = true;
			break;
		}


		setTypeColor();
	}

	void BossOrangeStopCherry::setStartPos(Vec2 pos) {
		startPos = pos;
	}

	void BossOrangeStopCherry::setTargetPos(Vec2 tPos) {
		targetPos = tPos;
		moveRangeX = targetPos.x - startPos.x;
		moveRangeY = targetPos.y - startPos.y;
	}

	// ----- 弾幕用水色りんご ----- //
	BossSkyTargetCherry::BossSkyTargetCherry(Vec2 startPos, double scale, BossCherryType cType) : BossBarrageCherry(startPos, scale, cType) {

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
		isDeleteOutOfScreen = true;
		isTrap = false;

		alpha = 1.0;
		startStep = 0;
	}

	void BossSkyTargetCherry::barrageUpdate() {
		setTypeColor();
	}
}
