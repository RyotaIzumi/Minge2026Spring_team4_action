#include "Cherry.h"
#include "../Audio/AudioAsset.h"
#include "../StageManager/StageManager.h"

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

		speed = 0;
		gravity = 0;
		direction = 0;

		depth = 50;

		//描画関連の決定
		hasAnimation = true;
		cherryTextureName = U"sprCherryLow";
		cherryColorType = CherryColorType::None;
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

		setTypeColor();
	}

	void Cherry::barrageUpdate() {
	}

	void Cherry::trapUpdate(int32 id) {
	}

	// 種類で色を決定する
	void Cherry::setTypeColor() {
		switch (cherryColorType) {
		case CherryColorType::None:   typeColor = ColorF(1.0, alpha); break;
		case CherryColorType::Red:    typeColor = ColorF(Palette::Red, alpha); break;
		case CherryColorType::Blue:   typeColor = ColorF(Palette::Blue, alpha); break;
		case CherryColorType::Yellow: typeColor = ColorF(Palette::Yellow, alpha); break;
		case CherryColorType::Green:  typeColor = ColorF(Palette::Lawngreen, alpha); break;
		case CherryColorType::Orange: typeColor = ColorF(Palette::Orange, alpha); break;
		case CherryColorType::Sky:    typeColor = ColorF(Palette::Skyblue, alpha); break;
			//case CherryColorType::Gray:   typeColor = ColorF(Palette::Gray, alpha); break;
		}
	}

	void Cherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		int32 texRange;
		if(hasAnimation)texRange = Periodic::Square0_1(0.5) * textureEdge;
		else texRange = 0;

		TextureAsset(cherryTextureName)(texRange,0,textureEdge,textureEdge).scaled(scaleMag).drawAt(pos.x, pos.y - 1,typeColor);
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


	// ----- りんごの見た目を作成時に設定可能 ----- //
	SpriteCherry::SpriteCherry(String name, Vec2 startPos, double scale) : Cherry(startPos,scale){
		const double side = 32;
		pos = { startPos.x * side, startPos.y * side };
		//描画関連の決定
		hasAnimation = true;
		cherryTextureName = name;
		cherryColorType = CherryColorType::None;
	}

	// ----- 罠りんご ----- //
	CherryTrap::CherryTrap(Vec2 startPos, int32 id, double dir, double spd) : Cherry(startPos,1.0), trapID(id) {
		pos = startPos;
		direction = dir;
		trapSpeed = spd;
		speed = 0;
		hspeed = 0;
		vspeed = 0;

		cherryType = CherryType::Trap;

		cherryTextureName = U"sprCherryLowWhite";
		cherryColorType = CherryColorType::Blue;

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

	// ----- 道中のギミックでかりんご ----- //
	GimmikBigCherry::GimmikBigCherry(Vec2 startPos, double scale, CherryColorType cType, StageManager& manager) : Cherry(startPos, scale) {

		//GameObject.hの値初期化
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);
		type = ObjectType::Cherry;
		cherryType = CherryType::Gimmik;
		cherryColorType = cType;

		stageManager = &manager;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = true;

		hasHp = false;

		speed = 0;
		direction = 0;
		gravity = 0;
	
		hasAnimation = true;
		cherryTextureName = U"sprCherryLowWhite";

		//初期起動時間決定
		switch (cherryColorType) {
		case CherryColorType::Red:    startTime = 2.2; break;
		case CherryColorType::Blue:   startTime = 2.2; break;
		case CherryColorType::Yellow:
			generateAttack();
			startTime = 1000000;
			attackInterval = 100000;
			attackIntervalStopwatch.reset();
			break;
		case CherryColorType::Green:  startTime = 0.1; break;
		case CherryColorType::Orange: startTime = 5.0; break;
		case CherryColorType::Sky:    startTime = 2.2; break;
		}

		switch (cherryColorType) {
		case CherryColorType::Red:    attackInterval = 1.0; break;
		case CherryColorType::Blue:   attackInterval = 2.0; break;
		case CherryColorType::Yellow: break;
		case CherryColorType::Green:  attackInterval = 3.8; break;
		case CherryColorType::Orange: attackInterval = 5.0; break;
		case CherryColorType::Sky:    attackInterval = 1.9; break;
		}
	}

	void GimmikBigCherry::barrageUpdate() {
		if (attackIntervalStopwatch.sF() > startTime) {
			generateAttack();
			startTime = 10000000;
			attackIntervalStopwatch.restart();
		}

		if (attackIntervalStopwatch.sF() > attackInterval) {
			generateAttack();
			attackIntervalStopwatch.restart();
		}
	}

	// 攻撃を呼び出す
	void GimmikBigCherry::generateAttack() {
			switch (cherryColorType) {
			case CherryColorType::Red:
				stageManager->createCherrySpread(20, 4, [this]() { return std::make_shared<BarrageCherry>(pos, 1.0,cherryColorType); });
				break;
			case CherryColorType::Blue:
				stageManager->createBlueLineCherry([this]() { return std::make_shared<BarrageGimmikBlueCherry>(pos, 1.0, cherryColorType); });
				break;
			case CherryColorType::Yellow:
				stageManager->createYellowStarCherry(5, 2, pos, 7, [this]() { return std::make_shared<BarrageGimmikYellowCherry>(pos, 1.0, cherryColorType); });
				break;
			case CherryColorType::Green:
				stageManager->createGreenWaveCherry(Vec2{-32,640}, 0.08, 18.5,[this]() { return std::make_shared<BarrageGimmikGreenCherry>(pos, 1.0, cherryColorType); });
				break;
			case CherryColorType::Orange:
				stageManager->createOrangeStopCherry(true, [this]() { return std::make_shared<BarrageGimmikOrangeCherry>(pos, 1.0, cherryColorType); });
				break;
			case CherryColorType::Sky:
				stageManager->createSkyTargetCherry(7, [this]() { return std::make_shared<BarrageCherry>(pos, 1.0, cherryColorType); });
				break;
			}
	}

	// ----- 弾幕用りんご ----- //
	BarrageCherry::BarrageCherry(Vec2 startPos, double scale, CherryColorType colorType) : Cherry(startPos, scale) {

		//GameObject.hの値初期化
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);
		cherryType = CherryType::Barrage;
		cherryColorType = colorType;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = true;

		alpha = 1.0;

		hasAnimation = false;
		cherryTextureName = U"sprCherryLowWhite";
	}

	void BarrageCherry::barrageUpdate() {
	}

	// ----- 弾幕用青りんご ----- //
	BarrageGimmikBlueCherry::BarrageGimmikBlueCherry(Vec2 startPos, double scale, CherryColorType colorType) : BarrageCherry(startPos, scale, colorType) {

		canPlayerKill = true;
		isDelete = false;
		isDeleteOutOfScreen = false;

		alpha = 1.0;
		startStep = 0;
	}

	void BarrageGimmikBlueCherry::barrageUpdate() {
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
	}

	// ----- 弾幕用黄りんご ----- //
	BarrageGimmikYellowCherry::BarrageGimmikYellowCherry(Vec2 startPos, double scale, CherryColorType colorType) : BarrageCherry(startPos, scale, colorType) {

		canPlayerKill = true;
		isDelete = false;
		isDeleteOutOfScreen = false;

		centerPos = startPos;

		alpha = 1.0;
		startStep = 0;
	}

	void BarrageGimmikYellowCherry::barrageUpdate() {
		switch (startStep) {
		case 0:
			r = calculateDistance(pos, centerPos);
			c = direction;
			startStep++;
			break;
		case 1:
			pos.x = r * cos(Math::ToRadians(c)) + centerPos.x;
			pos.y = -r * sin(Math::ToRadians(c)) + centerPos.y;
			c -= 0.3;
			break;
		}
	}

	// ----- 弾幕用緑りんご ----- //
	BarrageGimmikGreenCherry::BarrageGimmikGreenCherry(Vec2 startPos, double scale, CherryColorType cType) : BarrageCherry(startPos, scale, cType) {

		isDeleteOutOfScreen = false;

		alpha = 1.0;
		startStep = 0;
		speed = 0;

		gravity = 0.4;
	}

	void BarrageGimmikGreenCherry::barrageUpdate() {

		switch (startStep) {
		case 0:
			if (waveStopwatch.sF() > activeTimer) {
				direction = 90;
				speed = highSpeed;
				startStep++;
			}
			break;
		case 1:
			if (pos.y > 1000) {
				isDelete = true;
			}
			break;
		}

		setTypeColor();
	}

	void BarrageGimmikGreenCherry::setActiveTimer(double time) {
		activeTimer = time;
	}

	// ----- 弾幕用オレンジりんご ----- //
	BarrageGimmikOrangeCherry::BarrageGimmikOrangeCherry(Vec2 startPos, double scale, CherryColorType colorType) : BarrageCherry(startPos, scale, colorType) {

		cherryColorType = colorType;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = false;

		alpha = 1.0;
		startStep = 0;
		speed = 0;
	}

	void BarrageGimmikOrangeCherry::barrageUpdate() {

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

	void BarrageGimmikOrangeCherry::setStartPos(Vec2 pos) {
		startPos = pos;
	}

	void BarrageGimmikOrangeCherry::setTargetPos(Vec2 tPos) {
		targetPos = tPos;
		moveRangeX = targetPos.x - startPos.x;
		moveRangeY = targetPos.y - startPos.y;
	}
}
