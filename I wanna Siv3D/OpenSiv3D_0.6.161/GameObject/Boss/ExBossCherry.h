#pragma once
#include "../Cherry.h"

namespace Iwanna {
	class BossStageManager;

	class ExBossCherry : public Cherry {
	private:
		Vec2 baseCenterPos;
		double c, r;

		Stopwatch attackStopwatch{ StartImmediately::No };
		double attackIntervalTime;

		Timer startAttackTimer{ 0.8s, StartImmediately::Yes };

	protected:
		BossStageManager* bossStageManager = nullptr;
		int32 startStep = 0;
	public:
		ExBossCherry(Vec2 startPos, double scale, BossStageManager& manager);

		void barrageUpdate() override;
		void draw() const override;
	};

	class SordCherry : public BarrageCherry {
	private:
		Vec2 centerPos;
		double r, c;
	public:
		//SordCherry(Vec2 startPos, double scale, CherryColorType colorType);

		//void barrageUpdate() override;
	};

	class SordCherriesManager : public Cherry {
	private:
		BossStageManager* bossStageManager = nullptr;
	public:
		SordCherriesManager(Vec2 startPos, double scale, BossStageManager& manager);
		//void barrageUpdate() override;
		//void draw() const override;
	};
}
