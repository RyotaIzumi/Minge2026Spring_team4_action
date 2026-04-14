#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	class Warp : public GameObject {
	private:
		Vec2 size{ 32 , 32 };
	protected:
		String nextRoomName;
		bool canWarp;
	public:
		Warp(Vec2 startPos, String roomName);

		void update() override;
		void draw() const override;
		void onCollision(GameObject& other) override;
		String getNextRoomName() const;
		bool getCanWarp() const;
	};

	class SecretWarp : public Warp {
	private:
	public:
		SecretWarp(Vec2 startPos, String roomName);

		void update() override;
		void draw() const override;
	};

	class SecretWarpEffect : public GameObject {
	private:
	public:
		SecretWarpEffect(Vec2 startPos);

		void update() override;
		void draw() const override;
	};
}
