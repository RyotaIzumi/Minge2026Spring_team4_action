#include "Cherry.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	Cherry::Cherry(Vec2 startPos, double scale) {

		//GameObject.hの値初期化
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);
		type = ObjectType::Cherry;
		cherryType = CherryType::Normal;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isTrap = false;

		speed = 0;
		gravity = 0;
		direction = 0;
	}

	void Cherry::update() {
		checkOutOfScreen();
		updateForMoveTargetPos();
		barrageUpdate();

		if(isChangedDirOrSpd()) calculateSpeed();

		if (speed != 0 && !isMoving) {

			vspeed += gravity;

			// 位置更新
			pos.x += hspeed;
			pos.y += vspeed;
		}

		// 当たり判定位置更新
		hitBox->setPos(pos);

		//hp関連の処理
		if (hasHp) {
			if (mutekiInterval.reachedZero()) {
				isMuteki = false;
			}
		}
	}

	void Cherry::barrageUpdate() {
	}

	void Cherry::trapUpdate(int32 id) {
	}

	void Cherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLow").scaled(scaleMag).drawAt(pos.x, pos.y - 1);
		//hitBox->draw(Palette::Blue);//判定の可視化
	}


	void Cherry::movePosition(const Vec2& targetPoint, double timeSec, bool accele) {
		startPos = pos;
		targetPos = targetPoint;

		moveDuration = Math::Max(timeSec, 0.001); // 0除算防止
		moveElapsed = 0.0;

		isMoving = true;
		isAccelerationMode = accele;
	}

	void Cherry::updateForMoveTargetPos() {
		double dt = Scene::DeltaTime();
		if (!isMoving)
			return;

		moveElapsed += dt;

		double t = moveElapsed / moveDuration;
		t = Min(t, 1.0);
		double easedT = 0.0;

		// イージング（例：easeOutQuad）
		if (isAccelerationMode) {
			easedT = t * t;
		}
		else {
			easedT = 1.0 - (1.0 - t) * (1.0 - t);
		}


		pos = startPos.lerp(targetPos, easedT);

		if (t >= 1.0)
		{
			pos = targetPos;
			isMoving = false;
		}
	}

	bool Cherry::getIsMoveFinished() const {
		return !isMoving;
	}

	//画面外判定
	void Cherry::checkOutOfScreen() {
		const int32 excess = hitBoxSize * 2;//画面端からの余白
		if ((pos.x < -1 * excess || pos.x > Global::stageWidth + excess ||
			pos.y < -1 * excess || pos.y > Global::stageHeight + excess) &&
			isDeleteOutOfScreen) {
			isOutOfScreen = true;
		}
		else {
			isOutOfScreen = false;
		}
	}

	//hpを持つかどうか
	bool Cherry::getHasHp() const {
		return hasHp;
	}

	//ダメージを受けた際の処理
	void Cherry::hited() {
		if (hp > 0) {
			AudioAsset(Sound::BOSSHIT).playOneShot();
			hp--;
		}

		if (hp <= 0) {
			AudioAsset(Sound::DEATH).playOneShot();
			isDelete = true;
		}

		isMuteki = true;
		mutekiInterval.restart();
	}

	//無敵状態かどうか
	bool Cherry::getIsMuteki() const {
		return isMuteki;
	}

	// 自身の種類を取得
	CherryType Cherry::getCherryType() const {
		return cherryType;
	}

	void Cherry::onCollision(GameObject& other) {
	}

	CherryTrap::CherryTrap(Vec2 startPos, int32 id, double dir, double spd) : Cherry(startPos,1.0), trapID(id) {
		pos = startPos;
		direction = dir;
		trapSpeed = spd;
		speed = 0;
		hspeed = 0;
		vspeed = 0;

		cherryType = CherryType::Normal;

		isTrap = true;
		soundPlayOne = false;
	}

	void CherryTrap::trapUpdate(int32 id) {
		checkOutOfScreen();
		if (trapID == id) {
			isTrapActived = true;
		}

		if (isTrapActived) {
			speed = trapSpeed;

			//効果音再生
			if (!soundPlayOne) {
				AudioAsset(Sound::CHERRYFALL).playOneShot();
				soundPlayOne = true;
			}
		}
	}

	//罠のIDを取得
	int32 CherryTrap::getTrapID() const {
		return trapID;
	}
}
