#include "Spike.h"

namespace Iwanna {
	Spike::Spike(String typeName, Vec2 startPos, int32 dir) {
		this->typeName = typeName;
		pos.x = startPos.x * side;
		pos.y = startPos.y * side;
		spriteDirection = dir;
		hitBox = std::make_shared<SpikeHitBox>(pos, dir);
		type = ObjectType::Spike;
		canPlayerKill = true;
		alpha = 1.0;

		depth = 0;
	}

	void Spike::update() {
	}

	void Spike::trapUpdate(int32 id) {
	}

	void Spike::draw() const {
		switch (spriteDirection) {
		case 0: TextureAsset(U"sprSpikeUp_" + typeName).draw(pos, ColorF(1.0, alpha)); break;
		case 1: TextureAsset(U"sprSpikeLeft_" + typeName).draw(pos, ColorF(1.0, alpha)); break;
		case 2: TextureAsset(U"sprSpikeDown_" + typeName).draw(pos, ColorF(1.0, alpha)); break;
		case 3: TextureAsset(U"sprSpikeRight_" + typeName).draw(pos, ColorF(1.0, alpha)); break;
		}
	}

	void Spike::onCollision(GameObject& other) {
	}

	void Spike::checkOutOfScreen() {
		const int32 excess = side;
		if (pos.x < -1 * excess || pos.x > Global::stageWidth + excess ||
			pos.y < -1 * excess || pos.y > Global::stageHeight + excess) {
			isOutOfScreen = true;
		}
		else {
			isOutOfScreen = false;
		}
	}

	SpikeTrap::SpikeTrap(String typeName, Vec2 startPos, int32 dir, int32 id, double direction, double speed)
		: Spike(typeName, { startPos.x, startPos.y }, dir), trapID(id), direction(direction), speed(speed) {
		hspeed = 0;
		vspeed = 0;

		pos.x = startPos.x;
		pos.y = startPos.y;
		hitBox->setPos(pos);
	}

	void SpikeTrap::trapUpdate(int32 id) {
		checkOutOfScreen();
		if (trapID == id && !isTrapActived) {
			isTrapActived = true;
		}

		if (isTrapActived) {
			calculateSpeed();
			pos.x += hspeed;
			pos.y += vspeed;
		}

		hitBox->setPos(pos);
	}

	void SpikeTrap::calculateSpeed() {
		double rad = Math::ToRadians(direction);

		hspeed = speed * Math::Cos(rad);
		vspeed = -speed * Math::Sin(rad);
	}

	int32 SpikeTrap::getTrapID() const {
		return trapID;
	}

	SpikePathTrap::SpikePathTrap(String typeName, Vec2 startPos, int32 dir, int32 id, Vec2 next, double time)
		: Spike(typeName, { startPos.x, startPos.y }, dir), trapID(id) {
		hspeed = 0;
		vspeed = 0;
		nextGoalPos = { (startPos.x + next.x) * side, (startPos.y + next.y) * side };
		moveTime = time;
		velocity = (nextGoalPos - pos) / moveTime;
		hitBox->setPos(pos);
	}

	void SpikePathTrap::trapUpdate(int32 id) {
		checkOutOfScreen();

		if (trapID == id && !isTrapActived) {
			isTrapActived = true;

			elapsedTime = 0.0;
		}

		if (isTrapActived && !isTrapFinished) {
			double dt = Scene::DeltaTime();

			pos += velocity * dt;
			elapsedTime += dt;

			if (elapsedTime >= moveTime) {
				pos = nextGoalPos;
				isTrapFinished = true;
			}
		}
		hitBox->setPos(pos);
	}

	int32 SpikePathTrap::getTrapID() const {
		return trapID;
	}

	AppendSpike::AppendSpike(String typeName, Vec2 startPos, int32 dir)
		: Spike(typeName, { startPos.x, startPos.y }, dir) {
		alpha = 0.0;
		canPlayerKill = false;
	}

	void AppendSpike::update() {
		if (Global::isSecretTriggerActivated) {
			alpha += 0.02;
			canPlayerKill = true;
			if (alpha > 1.0) alpha = 1.0;
		}
	}

	DeleteSpike::DeleteSpike(String typeName, Vec2 startPos, int32 dir)
		: Spike(typeName, { startPos.x, startPos.y }, dir) {
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

	SpikeUpDown::SpikeUpDown(String typeName, Vec2 startPos, int32 dir, double time)
		: Spike(typeName, startPos, dir) {
		pos = startPos;
		basePos = pos;
		this->startPos = pos;
		hitBox = std::make_shared<SpikeHitBox>(pos, dir);
		moveTime = time;
	}

	void SpikeUpDown::update() {
		switch (moveStep) {
		case 0:
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
		case 1:
			moveAmount = moveSide * (moveTimer.sF() / moveTime);
			switch (spriteDirection) {
			case 0: pos.y = basePos.y + moveAmount; break;
			case 1: pos.x = basePos.x + moveAmount; break;
			case 2: pos.y = basePos.y - moveAmount; break;
			case 3: pos.x = basePos.x - moveAmount; break;
			}

			if (moveTimer.sF() >= moveTime) {
				moveAmount = 0;
				basePos = startPos;
				moveTimer.restart();
				moveStep = 0;
			}
			break;
		}
		hitBox->setPos(pos);
	}

	SpikeLoopMove::SpikeLoopMove(String typeName, Vec2 startPos, int32 dir, Vec2 moveAmount, double time)
		: Spike(typeName, startPos, dir) {
		pos = startPos;
		this->startPos = startPos;
		goalPos = startPos + moveAmount * side;
		moveTime = Max(time, 0.001);
		hitBox = std::make_shared<SpikeHitBox>(pos, dir);
	}

	void SpikeLoopMove::update() {
		elapsedTime += Scene::DeltaTime();

		while (elapsedTime >= moveTime) {
			elapsedTime -= moveTime;
			movingToGoal = !movingToGoal;
		}

		const double t = (elapsedTime / moveTime);
		if (movingToGoal) {
			pos = startPos + (goalPos - startPos) * t;
		}
		else {
			pos = goalPos + (startPos - goalPos) * t;
		}

		hitBox->setPos(pos);
	}
}
