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

		depth = 20;
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

			scale = 1.0 + 0.2 * Math::Sin(Scene::Time() * 3.0);

			if (alpha < 1.0)alpha += 0.02;

			if (generateWarpTimer.reachedZero()) {
				//エフェクト用ワープ生成
				warpEffects << std::make_shared<SecretWarpEffect>(pos);
				generateWarpTimer.restart();
			}

			for (auto& we : warpEffects) {
				we->update();
			}
			//削除
			warpEffects.remove_if([](auto&& we) {
				return we->isDelete;
			});
		}
	}
	void SecretWarp::draw() const {

		for (auto& we : warpEffects) {
			we->draw();
		}

		const Vec2 drawOffset = Vec2(16, 16);
		TextureAsset(U"sprSecretWarp").scaled(scale).drawAt(pos + drawOffset, ColorF(1.0, alpha));
		//hitBox->draw(ColorF(Palette::White,0.7));
	}

	// ↑の隠しマップワープ用エフェクト
	SecretWarpEffect::SecretWarpEffect(Vec2 startPos) {
		pos = startPos;
		direction = Random(360);
		speed = Random(1);
		alpha = 1.0;
		calculateSpeed();
	}

	void SecretWarpEffect::update() {
		pos.x += hspeed;
		pos.y += vspeed;

		if (alpha > 0)alpha -= 0.02;
		if (alpha <= 0)isDelete = true;
	}

	void SecretWarpEffect::draw() const {
		const Vec2 drawOffset = Vec2(16, 16);
		TextureAsset(U"sprSecretWarp").scaled(0.2).drawAt(pos + drawOffset, ColorF(1.0, alpha));
	}

	void SecretWarpEffect::onCollision(GameObject& other) {
	}
}
