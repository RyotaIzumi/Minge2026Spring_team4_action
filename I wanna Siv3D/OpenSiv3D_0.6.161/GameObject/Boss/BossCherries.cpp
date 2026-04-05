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
				speed = highSpeed;
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

	// ----- 弾幕用灰色りんご ----- //
	BossGrayLatticeCherry::BossGrayLatticeCherry(Vec2 startPos, double scale, BossCherryType cType) : BossBarrageCherry(startPos, scale, cType) {

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
		isDeleteOutOfScreen = false;
		isTrap = false;

		alpha = 0.5;
		speed = 0;
		startStep = 0;
	}

	void BossGrayLatticeCherry::barrageUpdate() {

		switch (attackPattern) {
		case 0://格子
			switch (attackStep) {
			case 0:
				alpha = 0.0;
				isDeleteOutOfScreen = true;
				circleTimer.restart();
				attackTimer.restart();
				attackStep++;
				break;
			case 1:
				if (circleTimer.reachedZero()) {
					if (r < showR)alpha = 1.0;
					alphaTimer.restart();
					attackStep++;
				}
				break;
			case 2:
				if (alpha > 0)alpha -= 0.01;
				if (alphaTimer.reachedZero() && r < showR) {
					alpha = 1.0;
					alphaTimer.restart();
				}
				if (attackTimer.reachedZero()) {
					alpha = 1.0;
					canPlayerKill = true;
					attackStep++;
				}
				break;
			case 3:
				canPlayerKill = false;
				alpha -= 0.05;
				if (alpha < 0)isDelete = true;
				break;
			}
			break;
		case 1://最初の予告円
			switch (attackStep) {
			case 0:
				alpha = 1.0;
				r = 0;
				circleTimer.restart();
				attackTimer.restart();
				attackStep++;
				break;
			case 1:
				r = showR * EaseOutQuad(circleTimer.progress0_1());
				if (circleTimer.reachedZero()) {
					attackStep++;
				}
				break;
			case 2:
				addAngleValue += 0.01;
				c += addAngleValue;
				if (attackTimer.reachedZero()) {
					attackStep++;
				}
				break;
			case 3:
				alpha -= 0.05;
				if (alpha < 0)isDelete = true;
				break;
			}
			break;
		case 2://2つ目の予告円
			switch (attackStep) {
			case 0:
				alpha = 0.3;
				r = 0;
				attackTimer.restart();
				attackStep++;
				break;
			case 1:
				r = showR * attackTimer.progress0_1();
				addAngleValue -= 0.01;
				c += addAngleValue;

				if (attackTimer.reachedZero()) {
					attackStep++;
				}
				break;
			case 2:
				alpha -= 0.01;
				if (alpha < 0)isDelete = true;
				break;
			}
			break;
		}

		pos.x = r * cos(Math::ToRadians(c)) + centerPos.x;
		pos.y = -r * sin(Math::ToRadians(c)) + centerPos.y;

		setTypeColor();
	}

	void BossGrayLatticeCherry::setCenterPos(Vec2 cPos) {
		centerPos = cPos;
	}

	void BossGrayLatticeCherry::setDistanceAndAngle(double r, double c) {
		this->c = c;
		this->r = r;
	}

	void BossGrayLatticeCherry::setAttackPattern(int32 pattern) {
		attackPattern = pattern;
	}
}
