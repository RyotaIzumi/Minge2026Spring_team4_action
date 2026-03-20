#include "SavePoint.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	SavePoint::SavePoint(Vec2 startPos) {
		//GameObject.hの値初期化
		pos = { startPos.x * side, startPos.y * side };
		hitBox = std::make_shared<RectHitBox>(Vec2{ pos.x,pos.y + gapY }, SizeF{ hitBoxSize });
		type = ObjectType::SavePoint;
		saveType = SaveType::Normal;
		canPlayerKill = false;
	}
	void SavePoint::update() {
		if(isSaving && saveIntervalTimer.reachedZero()) {
			isSaving = false;
		}
	}
	void SavePoint::trapUpdate(int32 id) {
	}
	void SavePoint::draw() const {
		//hitBox->draw(Palette::Gray);
		TextureAsset(U"sprSave")(isSaving ? side : 0, 0, side, side).draw(pos);
	}
	// セーブされたときの処理
	void SavePoint::saved() {
		if (!isSaving) {
			isSaving = true;
			saveIntervalTimer.restart();

			if (onSavedCallback) {
				onSavedCallback();
			}
		}
	}
	
	void SavePoint::onCollision(GameObject& other) {
	}

	//罠用かどうかを取得
	bool SavePoint::getIsTrap() const{
		return isTrap;
	}

	//罠ID取得
	int32 SavePoint::getTrapID() const{
		return trapID;
	}

	// ----- 以下別種類のセーブブロック ----- //
	//(移動セーブトラップ)

	SaveMoveTrap::SaveMoveTrap(Vec2 startPos, int32 id, double spd, double dir) : SavePoint(startPos){
		pos = startPos;
		trapID = id;
		speed = spd;
		direction = dir;

		hitBox = std::make_shared<RectHitBox>(Vec2{ pos.x,pos.y + gapY }, SizeF{ hitBoxSize });
		type = ObjectType::SavePoint;
		saveType = SaveType::MoveTrap;

		isTrap = true;
		canPlayerKill = false;
		isStartTrap = false;
	}

	void SaveMoveTrap::trapUpdate(int32 id) {
		if (trapID == id && !isStartTrap) {
			isStartTrap = true;
			canPlayerKill = true;
			AudioAsset(Sound::SPIKETRAP).playOneShot();
		}

		if (isStartTrap) {
			if (pos.y < -32)return;
			calculateSpeed();
			pos.x += hspeed;
			pos.y += vspeed;
			hitBox->setPos({ pos.x + hitBoxSize.x / 2, pos.y + hitBoxSize.y / 2 + gapY });
		}
	}

	bool SaveMoveTrap::getIsStartTrap() const {
		return isStartTrap;
	}

	//(偽セーブトラップ)
	SaveFakeTrap::SaveFakeTrap(Vec2 startPos) : SavePoint(startPos) {
		pos = startPos;

		hitBox = std::make_shared<RectHitBox>(Vec2{ pos.x,pos.y + gapY }, SizeF{ hitBoxSize });
		type = ObjectType::SavePoint;
		saveType = SaveType::FakeTrap;

		isTrap = true;
		canPlayerKill = false;
		isStartTrap = false;
	}

	void SaveFakeTrap::trapUpdate(int32 id) {
		//idは使わない
		if (isStartTrap) {
			switch (trapStep) {
			case 0:
				Global::isPlayerFrozen = true;
				Global::bgmStop = true;
				trapIntervalTimer.restart();

				trapStep++;
				break;
			case 1:
				if (trapIntervalTimer.reachedZero()) {
					Global::trapCameraActivatedInTrap2Map = true;
					trapStep++;
				}
				break;
			}
		}
	}

	void SaveFakeTrap::setIsStartTrap(bool b) {
		isStartTrap = b;
	}

	bool SaveFakeTrap::getIsStartTrap() const {
		return isStartTrap;
	}

	void SaveFakeTrap::draw() const {
		TextureAsset(U"sprFakeSave")(0, 0, side, side).draw(pos);
		if (Global::trapCameraActivatedInTrap2Map)
			FontAsset(U"Button")(U"↓").drawAt(pos.x + 16,pos.y - 20,ColorF(Palette::Black));
	}
}
