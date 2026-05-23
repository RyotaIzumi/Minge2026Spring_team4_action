#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	class Spike : public GameObject {
	private:
	protected:
		String typeName;
		int32 side = 32;
		int32 spriteDirection;//針の向き
	public:
		Spike(String typeName, Vec2 startPos, int32 dir);

		bool isOutOfScreen = false;

		void update() override;
		virtual void trapUpdate(int32 id);
		void draw() const override;
		void onCollision(GameObject& other) override;
		void checkOutOfScreen();
	};

	class SpikeTrap : public Spike {
	private:
		int32 trapID = 0;
		double direction = 0;
		double speed = 0;
		bool isTrapActived = false;
	public:
		SpikeTrap(String typeName, Vec2 startPos, int32 dir, int32 id, double direction, double speed);
		void trapUpdate(int32 id) override;
		void calculateSpeed();

		int32 getTrapID() const;
	};

	class SpikePathTrap : public Spike {
	private:
		int32 trapID = 0;
		double speed = 0;
		Vec2 nextGoalPos;
		bool isTrapActived = false;
		bool isTrapFinished = false;

		double moveTime = 2.0;      // 到達秒数
		double elapsedTime = 0.0;   // 経過時間
		Vec2 velocity;              // 移動ベクトル
	public:
		SpikePathTrap(String typeName, Vec2 startPos, int32 dir, int32 id, Vec2 next, double time);
		void trapUpdate(int32 id) override;
	};

	class AppendSpike : public Spike {
	public:
		AppendSpike(String typeName, Vec2 startPos, int32 dir);
		void update() override;
	};

	class DeleteSpike : public Spike {
	public:
		DeleteSpike(String typeName, Vec2 startPos, int32 dir);
		void update() override;
	};

	class SpikeUpDown : public Spike {
	private:
		int32 moveStep = 0;//移動の段階を管理する変数
		double moveSide = 32;
		double moveAmount = 0.0;//移動量を管理する変数
		double moveTime = 1.0;
		Vec2 basePos,startPos;
		Stopwatch moveTimer{ StartImmediately::Yes };
	public:
		SpikeUpDown(String typeName, Vec2 startPos, int32 dir, double time);

		void update() override;
	};
}
