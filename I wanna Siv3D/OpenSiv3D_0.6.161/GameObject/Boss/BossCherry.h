#pragma once
#include "../Cherry.h"

namespace Iwanna {
	class BossStageManager;
	enum class BossCherryType {
		Red,
		Blue,
		Yellow,
		Green,
		Orange,
		Sky,
		None
	};

	class BossCherry : public Cherry {
	private:
		Vec2 baseCenterPos;
		double c, r;

		Stopwatch attackStopwatch{StartImmediately::No};
		double attackIntervalTime;
		
	protected:
		BossStageManager* bossStageManager = nullptr;
		BossCherryType cherryAttackType;
		int32 startStep = 0;
	public:
		BossCherry(Vec2 startPos, double scale, BossStageManager& manager);

		void barrageUpdate() override;
		void draw() const override;

		BossCherryType getBossCherryAttackType() const;
		bool reachedAttackTime(double time);
	};

	class BossSubCherry : public Cherry {
	private:
		BossStageManager* bossStageManager = nullptr;
		BossCherryType cherrySubType;
		BossCherryType cherryAttackType;
		int32 startStep = 0;

		Vec2 centerPos;
		double c,r,rMax;

		Timer startTimer{ 2.3s, StartImmediately::Yes };
		Timer rTimer{ 0.7s };
	public:
		BossSubCherry(Vec2 startPos, double scale, BossCherryType cType, BossStageManager& manager);

		void barrageUpdate() override;
		void draw() const override;

		void setCenterPos(Vec2 cPos);
		void generateAttack(BossCherryType);
	};

	class BossBarrageCherry : public Cherry {
	protected:
		int32 startStep = 0;
		BossCherryType cherrySubType;
	public:
		BossBarrageCherry(Vec2 startPos, double scale, BossCherryType cType);

		void barrageUpdate() override;
		void draw() const override;
	};
	
	class BossSubThrowCherry : public BossBarrageCherry {
	private:
		BossStageManager* bossStageManager = nullptr;
		Timer splitTimer{ 1.0s ,StartImmediately::Yes };
	public:
		BossSubThrowCherry(Vec2 startPos, double scale, BossCherryType cType, BossStageManager& manager);

		void barrageUpdate() override;
		void draw() const override;

		void split();
	};
}
