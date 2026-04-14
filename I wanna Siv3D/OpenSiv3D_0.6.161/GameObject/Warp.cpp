#include "Warp.h"

namespace Iwanna {
	Warp::Warp(Vec2 startPos, String roomName) {
		//GameObject.hの値初期化
		pos = startPos;
		nextRoomName = roomName;

		hitBox = std::make_shared<RectHitBox>(pos,size);
		type = ObjectType::Warp;
		canPlayerKill = false;
		canWarp = true;
	}
	void Warp::update() {
	}
	void Warp::draw() const {
		TextureAsset(U"sprWarp").draw(pos);
		//hitBox->draw(ColorF(Palette::White,0.7));
	}
	void Warp::onCollision(GameObject& other) {
	}
	String Warp::getNextRoomName() const {
		return nextRoomName;
	}
	bool Warp::getCanWarp() const {
		return canWarp;
	}

	// ----- アイテム部屋用ワープ ----- //
	SecretWarp::SecretWarp(Vec2 startPos, String roomName) : Warp(startPos, roomName) {
		canWarp = false;
		alpha = 0.0;
	}
	void SecretWarp::update() {
		if (Global::isSecretTriggerActivated) {
			canWarp = true;

			if (alpha < 1.0)alpha += 0.02;
		}
	}
	void SecretWarp::draw() const {
		TextureAsset(U"sprSecretWarp").draw(pos,ColorF(1.0,alpha));
		//hitBox->draw(ColorF(Palette::White,0.7));
	}
}
