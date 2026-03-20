#include "Spike.h"

namespace Iwanna {
	Spike::Spike(Vec2 startPos, int32 dir) {
		//GameObject.hの値初期化
		pos.x = startPos.x * side;
		pos.y = startPos.y * side;
		spriteDirection = dir;
		hitBox = std::make_shared<SpikeHitBox>(pos, dir);
		type = ObjectType::Spike;
		canPlayerKill = true;
	}

	void Spike::update() {
	}

	void Spike::trapUpdate(int32 id) {
	}

	void Spike::draw() const {
		switch (spriteDirection) {
		case 0:TextureAsset(U"sprSpikeUp_low").draw(pos); break;
		case 1:TextureAsset(U"sprSpikeLeft_low").draw(pos); break;
		case 2:TextureAsset(U"sprSpikeDown_low").draw(pos); break;
		case 3:TextureAsset(U"sprSpikeRight_low").draw(pos); break;
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

	SpikeTrap::SpikeTrap(Vec2 startPos, int32 dir, int32 id, double direction, double speed) : Spike({ startPos.x, startPos.y }, dir), trapID(id), direction(direction), speed(speed) {
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
	SpikePathTrap::SpikePathTrap(Vec2 startPos, int32 dir, int32 id, Vec2 next, double time) : Spike({ startPos.x, startPos.y }, dir), trapID(id) {
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
}
