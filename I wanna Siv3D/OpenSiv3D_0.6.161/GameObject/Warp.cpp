#include "Warp.h"

namespace Iwanna {
	Warp::Warp(Vec2 startPos, String roomName) {
		//GameObject.hの値初期化
		pos = startPos;
		nextRoomName = roomName;

		hitBox = std::make_shared<RectHitBox>(pos,size);
		type = ObjectType::Warp;
		canPlayerKill = false;
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
}
