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
		Gray,
		None
	};

	class BossCherry : public Cherry {
	private:
		Vec2 baseCenterPos;
		double c, r;

		Stopwatch attackStopwatch{StartImmediately::No};
		double attackIntervalTime;

		Timer startAttackTimer{ 0.8s, StartImmediately::Yes };

		//特殊攻撃関連
		bool isSpecialAttack = false;
		int32 specialAttackStep = 0;

		//hpバー用
		double hpBarAlpha = 0.0;

		Stopwatch specialAttackStopwatch{ StartImmediately::No };
		double specialAttackIntervalTime = 8.0;
		int32 specialAttackCount = 0;
		
	protected:
		BossStageManager* bossStageManager = nullptr;
		BossCherryType cherryAttackType;
		Array<BossCherryType> canAttackTypes{ BossCherryType::Red, BossCherryType::Blue, BossCherryType::Yellow, BossCherryType::Green, BossCherryType::Orange, BossCherryType::Sky };
		int32 startStep = 0;
		int32 defeatedAttackTypeNum = 0;
	public:
		BossCherry(Vec2 startPos, double scale, BossStageManager& manager);

		void barrageUpdate() override;
		void draw() const override;

		void startAttack(BossCherryType type);
		BossCherryType getBossCherryAttackType() const;
		void removeDefeatedAttackType(BossCherryType type);
		int32 getDefeatedBossNum() const;
		bool reachedAttackTime(double time);

		void hited() override;
	};

	class BossSubCherry : public Cherry {
	private:
		BossStageManager* bossStageManager = nullptr;
		BossCherryType cherrySubType;
		BossCherryType cherryAttackType;
		int32 startStep = 0;
		int32 defeatedBossNum = 0;
		ColorF typeColor;

		Vec2 centerPos;
		double c,r,rMax;

		Timer startTimer{ 2.3s, StartImmediately::Yes };
		Timer rTimer{ 0.7s };
	public:
		BossSubCherry(Vec2 startPos, double scale, BossCherryType cType, BossStageManager& manager);

		void barrageUpdate() override;
		void draw() const override;

		void setCenterPos(Vec2 cPos);
		void setTypeColor();
		void setDefeatedBossNum(int32);
		BossCherryType getBossCherrySubType() const;
		void generateAttack(BossCherryType);
	};

	class BossBarrageCherry : public Cherry {
	protected:
		int32 startStep = 0;
		BossCherryType cherrySubType;
		ColorF typeColor;
	public:
		BossBarrageCherry(Vec2 startPos, double scale, BossCherryType cType);

		void barrageUpdate() override;
		void draw() const override;

		void setTypeColor();
	};

	class BossFallBlueCherry : public BossBarrageCherry {
	protected:
		int32 startStep = 0;
		BossCherryType cherrySubType;
		ColorF typeColor;
	public:
		BossFallBlueCherry(Vec2 startPos, double scale, BossCherryType cType);

		void barrageUpdate() override;
	};

	class BossYellowStarCherry : public BossBarrageCherry {
	protected:
		int32 startStep = 0;
		BossCherryType cherrySubType;
		ColorF typeColor;
		Vec2 centerPos;
	public:
		BossYellowStarCherry(Vec2 startPos, double scale, BossCherryType cType);

		void barrageUpdate() override;
	};

	class BossGreenWaveCherry : public BossBarrageCherry {
	protected:
		int32 startStep = 0;
		BossCherryType cherrySubType;
		ColorF typeColor;

		Stopwatch waveStopwatch{ StartImmediately::Yes };
		double activeTimer = 0.0;
	public:
		BossGreenWaveCherry(Vec2 startPos, double scale, BossCherryType cType);
		double highSpeed = 10;

		void barrageUpdate() override;
		void setActiveTimer(double time);
	};

	class BossOrangeStopCherry : public BossBarrageCherry {
	protected:
		int32 startStep = 0;
		BossCherryType cherrySubType;
		ColorF typeColor;

		Vec2 startPos;
		Vec2 targetPos;
		double moveRangeX, moveRangeY;
		Timer moveTimer{ 0.5s, StartImmediately::Yes };
	public:
		BossOrangeStopCherry(Vec2 startPos, double scale, BossCherryType cType);

		void barrageUpdate() override;
		void setStartPos(Vec2 tPos);
		void setTargetPos(Vec2 tPos);
	};

	class BossSkyTargetCherry : public BossBarrageCherry {
	protected:
		int32 startStep = 0;
		BossCherryType cherrySubType;
		ColorF typeColor;
	public:
		BossSkyTargetCherry(Vec2 startPos, double scale, BossCherryType cType);

		void barrageUpdate() override;
	};

	class BossGrayLatticeCherry : public BossBarrageCherry {
	protected:
		int32 startStep = 0;
		BossCherryType cherrySubType;
		ColorF typeColor;
		Vec2 centerPos;
		double c, r;
		double showR = 200;
		int32 attackStep = 0;
		int32 attackPattern = 0;//0:格子,1:最初の円,2:最後の円

		double addAngleValue = 0.0;
		Timer circleTimer{ 1.0s, StartImmediately::No };
		Timer attackTimer{ 5.0s, StartImmediately::No };
		Timer alphaTimer{ 1.0s, StartImmediately::No };
	public:
		BossGrayLatticeCherry(Vec2 startPos, double scale, BossCherryType cType);

		void barrageUpdate() override;
		void setCenterPos(Vec2 cPos);
		void setDistanceAndAngle(double c, double r);
		void setAttackPattern(int32 pattern);
	};

	class BossSubThrowCherry : public BossBarrageCherry {
	private:
		BossStageManager* bossStageManager = nullptr;
		Timer splitTimer{ 1.0s ,StartImmediately::Yes };

		//回転関連
		bool isRorateLeft = true;
		double rotateC = 0.0;
		double addRorateC = 0.17;
	public:
		BossSubThrowCherry(Vec2 startPos, double scale, BossCherryType cType, BossStageManager& manager);

		void barrageUpdate() override;
		void draw() const override;

		void split();
	};
}
