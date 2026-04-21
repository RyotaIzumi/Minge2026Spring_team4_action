#pragma once
#include "../Cherry.h"

namespace Iwanna {
	class BossStageManager;

	class SordCherry : public BarrageCherry {
	private:
		Vec2 sordBaseCenterPos;
		double r, c;
		bool isFollowBoss = false;
	public:
		SordCherry(Vec2 startPos, double scale, CherryColorType colorType);

		void barrageUpdate() override;
		void setSordBaseCenterPos(Vec2);
	};

	class SordCherriesManager : public Cherry {
	private:
		Array<std::shared_ptr<SordCherry>> sordCherries;
		BossStageManager* bossStageManager = nullptr;
	public:
		SordCherriesManager(Vec2 startPos, double scale, BossStageManager& manager);
		void createSordCherries();
		void barrageUpdate() override;
		void setSordBaseCenterPos(Vec2 pos);
		void draw() const override;
	};

	class ExBossCherry : public Cherry {
	private:
		Vec2 baseCenterPos;
		double c, r;
		bool isFollowBoss = false;

		Stopwatch attackStopwatch{ StartImmediately::No };
		double attackIntervalTime;

		Timer startAttackTimer{ 0.8s, StartImmediately::Yes };

	protected:
		BossStageManager* bossStageManager = nullptr;
		SordCherriesManager* sordCherriesManager = nullptr;
		int32 startStep = 0;
	public:
		ExBossCherry(Vec2 startPos, double scale, BossStageManager& manager);

		void barrageUpdate() override;
		void draw() const override;
	};
}
