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

		depth = 10;
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
		const StringView textureName = (Global::mainTextureNumber == 0) ? U"sprSave_low" : U"sprSave_normal";
		TextureAsset(textureName)(isSaving ? side : 0, 0, side, side).draw(pos);
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

	//セーブポイントの種類を取得
	SaveType SavePoint::getSaveType() const {
		return saveType;
	}

	// ----- 以下別種類のセーブブロック ----- //
	//(ボス開始用セーブ)
	BossSavePoint::BossSavePoint(Vec2 startPos, int32 id) : SavePoint(startPos) {
		//GameObject.hの値初期化
		pos = { startPos };
		appendBossId = id;

		const Vec2 centerOffset = { 16,16 };
		hitBox = std::make_shared<RectHitBox>(Vec2{ pos.x,pos.y + gapY } - centerOffset, SizeF{ hitBoxSize });
		type = ObjectType::SavePoint;
		saveType = SaveType::Boss;
		canPlayerKill = false;
	}
	void BossSavePoint::update() {
		if (isSaving) {
			saveAlpha -= 0.1;
			saveScale += 0.1;
			saveRotate += 0.11;

			if (saveAlpha < 0) {
				isDelete = true;
			}
		}
	}
	void BossSavePoint::draw() const {
		//hitBox->draw(ColorF(0.7,0.7));
		const StringView textureName = (Global::mainTextureNumber == 0) ? U"sprSave_low" : U"sprSave_normal";
		TextureAsset(textureName)(isSaving ? side : 0, 0, side, side).scaled(saveScale).rotated(saveRotate).drawAt(pos, ColorF(1.0, saveAlpha));
	}

	int32 BossSavePoint::getAppendBossId() const {
		return appendBossId;
	}
	 
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
				Global::trap2MapBgmStop = true;
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

	// ----- 隠しアイテム部屋用セーブポイント ----- //
	SecretSavePoint::SecretSavePoint(Vec2 startPos,String roomName) : SavePoint(startPos) {
		type = ObjectType::SavePoint;
		saveType = SaveType::Secret;
		escapeRoomName = roomName;
	}

	void SecretSavePoint::update() {
		if (isSaving && saveIntervalTimer.reachedZero()) {
			isSaving = false;
		}

		if (Global::isSecretTriggerActivated) {
			alpha -= 0.02;
			if (alpha < 0) isDelete = true;
		}

		if (textAlpha < 1.0 && isPlayerTouching)textAlpha += 0.04;
		else if(textAlpha >= 0.0)textAlpha -= 0.04;
	}

	void SecretSavePoint::draw() const {
		const StringView textureName = (Global::mainTextureNumber == 0) ? U"sprSave_low" : U"sprSave_normal";
		TextureAsset(textureName)(isSaving ? side : 0, 0, side, side).draw(pos,ColorF(1.0,alpha));

		// 文字表示
		Vec2 textBasePos = Vec2(pos.x + 16, pos.y - 16);
		FontAsset(U"PlayerMessage")(U"Press Q to escape").drawAt(textBasePos.x - 1,textBasePos.y, ColorF(Palette::Black,textAlpha));
		FontAsset(U"PlayerMessage")(U"Press Q to escape").drawAt(textBasePos.x + 1,textBasePos.y, ColorF(Palette::Black,textAlpha));
		FontAsset(U"PlayerMessage")(U"Press Q to escape").drawAt(textBasePos.x,textBasePos.y + 1, ColorF(Palette::Black,textAlpha));
		FontAsset(U"PlayerMessage")(U"Press Q to escape").drawAt(textBasePos.x,textBasePos.y - 1, ColorF(Palette::Black,textAlpha));

		FontAsset(U"PlayerMessage")(U"Press Q to escape").drawAt(textBasePos, ColorF(Palette::Whitesmoke, textAlpha));
	}

	String SecretSavePoint::getEscapeRoomName() const {
		return escapeRoomName;
	}
}
