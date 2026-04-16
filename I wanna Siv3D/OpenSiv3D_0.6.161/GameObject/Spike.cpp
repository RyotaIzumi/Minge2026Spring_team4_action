#include "Spike.h"

namespace Iwanna {
	Spike::Spike(String typeName, Vec2 startPos, int32 dir) {
		this->typeName = typeName;
		//GameObject.hの値初期化
		pos.x = startPos.x * side;
		pos.y = startPos.y * side;
		spriteDirection = dir;
		hitBox = std::make_shared<SpikeHitBox>(pos, dir);
		type = ObjectType::Spike;
		canPlayerKill = true;
		alpha = 1.0;
	}

	void Spike::update() {
	}

	void Spike::trapUpdate(int32 id) {
	}

	void Spike::draw() const {
		switch (spriteDirection) {
		case 0:TextureAsset(U"sprSpikeUp_" + typeName).draw(pos,ColorF(1.0,alpha)); break;
		case 1:TextureAsset(U"sprSpikeLeft_" + typeName).draw(pos, ColorF(1.0, alpha)); break;
		case 2:TextureAsset(U"sprSpikeDown_" + typeName).draw(pos, ColorF(1.0, alpha)); break;
		case 3:TextureAsset(U"sprSpikeRight_" + typeName).draw(pos, ColorF(1.0, alpha)); break;
		}
		
		//hitBox->draw(ColorF(Palette::Blue,0.7));
	}

	void Spike::onCollision(GameObject& other) {
	}

	//画面外判定
	void Spike::checkOutOfScreen() {
		const int32 excess = side;//画面端からの余白
		if (pos.x < -1 * excess || pos.x > Global::stageWidth + excess ||
			pos.y < -1 * excess || pos.y > Global::stageHeight + excess) {
			isOutOfScreen = true;
		}
		else {
			isOutOfScreen = false;
		}
	}

	// ----- 以下罠用針 ----- //

	SpikeTrap::SpikeTrap(String typeName, Vec2 startPos, int32 dir, int32 id, double direction, double speed) : Spike(typeName, { startPos.x, startPos.y }, dir), trapID(id), direction(direction), speed(speed) {
		hspeed = 0;
		vspeed = 0;
	}

	void SpikeTrap::trapUpdate(int32 id) {
		checkOutOfScreen();
		if (trapID == id) {
			isTrapActived = true;
		}

		if (isTrapActived) {
			calculateSpeed();
			pos.x += hspeed;
			pos.y += vspeed;
			hitBox->setPos(pos);
		}
	}

	//speedとdirからhspeedとvspeedを計算
	void SpikeTrap::calculateSpeed() {
		//ラジアンに変換
		double rad = Math::ToRadians(direction);

		hspeed = speed * Math::Cos(rad);
		vspeed = -speed * Math::Sin(rad);
	}

	//罠のIDを取得
	int32 SpikeTrap::getTrapID() const {
		return trapID;
	}

	//特定の箇所へ移動する針トラップ
	SpikePathTrap::SpikePathTrap(String typeName, Vec2 startPos, int32 dir, int32 id, Vec2 next, double time) : Spike(typeName, { startPos.x, startPos.y }, dir), trapID(id) {
		hspeed = 0;
		vspeed = 0;
		nextGoalPos = { (startPos.x + next.x) * side, (startPos.y + next.y) * side };
		moveTime = time;
		velocity = (nextGoalPos - pos) / moveTime;
	}

	void SpikePathTrap::trapUpdate(int32 id) {
		checkOutOfScreen();

		if (trapID == id && !isTrapActived) {
			isTrapActived = true;

			elapsedTime = 0.0;
		}

		if (isTrapActived && !isTrapFinished) {
			double dt = Scene::DeltaTime();  // Siv3Dのフレーム時間

			pos += velocity * dt;
			elapsedTime += dt;

			if (elapsedTime >= moveTime) {
				pos = nextGoalPos;      // 誤差補正
				isTrapFinished = true;  // 必要なら停止
			}
		}
		hitBox->setPos(pos);
	}

	// ----- 途中で出現する針 ----- //
	AppendSpike::AppendSpike(String typeName, Vec2 startPos, int32 dir) : Spike(typeName, { startPos.x, startPos.y }, dir) {
		alpha = 0.0;
		canPlayerKill = false;
	}

	void AppendSpike::update() {
		if(Global::isSecretTriggerActivated) {
			alpha += 0.02;
			canPlayerKill = true;
			if (alpha > 1.0) alpha = 1.0;
		}
	}

	// ----- 途中で消える針 ----- //
	DeleteSpike::DeleteSpike(String typeName, Vec2 startPos, int32 dir) : Spike(typeName, { startPos.x, startPos.y }, dir) {
		alpha = 1.0;
		canPlayerKill = true;
	}

	void DeleteSpike::update() {
		if (Global::isSecretTriggerActivated) {
			alpha -= 0.02;
			canPlayerKill = false;
			if (alpha < 0.0) isDelete = true;
		}
	}

	// ----- 昇降針 ----- //
	SpikeUpDown::SpikeUpDown(String typeName, Vec2 startPos, int32 dir, double time) : Spike(typeName, startPos, dir) {
		pos = startPos;
		basePos = pos;
		hitBox = std::make_shared<SpikeHitBox>(pos, dir);
		moveTime = time;
	}

	void SpikeUpDown::update() {
		switch (moveStep) {
		case 0://昇
			if (moveTimer.sF() >= moveTime) {
				moveAmount = 0;
				basePos = pos;
				moveTimer.restart();
				moveStep++;
			}

			moveAmount = moveSide * (moveTimer.sF() / moveTime);
			switch (spriteDirection) {
				case 0: pos.y = basePos.y - moveAmount; break;
				case 1: pos.x = basePos.x - moveAmount; break;
				case 2: pos.y = basePos.y + moveAmount; break;
				case 3: pos.x = basePos.x + moveAmount; break;
			}
			break;
		case 1://降
			moveAmount = moveSide * (moveTimer.sF() / moveTime);
			switch (spriteDirection) {
			case 0: pos.y = basePos.y + moveAmount; break;
			case 1: pos.x = basePos.x + moveAmount; break;
			case 2: pos.y = basePos.y - moveAmount; break;
			case 3: pos.x = basePos.x - moveAmount; break;
			}

			if (moveTimer.sF() >= moveTime) {
				moveAmount = 0;
				basePos = pos;
				moveTimer.restart();
				moveStep = 0;
			}
			break;
		}

		
		


		hitBox->setPos(pos);
	}
}
