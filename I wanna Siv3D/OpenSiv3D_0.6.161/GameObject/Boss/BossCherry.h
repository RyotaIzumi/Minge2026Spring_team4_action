#pragma once
#include "../Cherry.h"

namespace Iwanna {
	class BossStageManager;
	class BossBarrageCherry;
	class BossSkyTargetCherry;
	class TayamaLineCherry;
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

	class TayamaBoss : public Cherry {
	private:
		BossStageManager* bossStageManager = nullptr;
		double hpBarAlpha = 0.0;
		double baseScaleMag = 1.0;
		bool isDefeatedFall = false;
		int32 appearanceStep = 0;
		int32 currentAttackPattern = 0;
		int32 attackCountInSet = 0;
		int32 targetAttackCountInSet = 0;
		int32 lineAttackStep = 0;

		Stopwatch attackStopwatch{ StartImmediately::No };
		Stopwatch attackIntervalStopwatch{ StartImmediately::No };

		double attackWaitTime = 2.0;
		double rotatingSpreadAttackDuration = 5.0;
		double rotatingSpreadAttackInterval = 0.18;
		double rotatingSpreadRotateSpeed = 800.0;
		int32 rotatingSpreadCherryNum = 6;
		double rotatingSpreadCherrySpeed = 6.0;
		double targetAttackDuration = 8.0;
		double targetAttackInterval = 0.1;
		int32 targetAttackLineNum = 1;
		bool targetAttackIsAddLine = false;
		double targetAttackBaseSpeed = 7.0;
		double targetAttackIntervalSpeed = 5.0;
		double lineAttackMoveDuration = 1.5;
		double lineAttackTargetY = 160.0;
		double lineAttackCherryInterval = 20.0;
		double lineAttackWarnScale = 1.2;
		double lineAttackScaleUpTime = 0.25;
		double lineAttackScaleDownTime = 0.5;
		double lineAttackGenerateWaitTime = 0.35;
		double defeatedFallSpeed = 0.0;
		double defeatedFallAcceleration = 0.25;
		double defeatedRotateSpeed = 90.0;

		void applyScaleMag(double scale);
		void updateDefeatedFall();
		void startRandomAttack();
		void startRotatingSpreadAttack();
		void updateRotatingSpreadAttack();
		void startTargetAttack();
		void updateTargetAttack();
		void startLineAttack();
		void updateLineAttack();
		void generateLineAttack();
		std::shared_ptr<BossBarrageCherry> createTrapBarrageCherry();
		std::shared_ptr<BossSkyTargetCherry> createTrapSkyTargetCherry();
		std::shared_ptr<TayamaLineCherry> createTrapLineCherry(Vec2 startPos, Vec2 targetPos);
		void finishAttack();

	public:
		TayamaBoss(Vec2 startPos, double scale, BossStageManager& manager);

		void setScaleMag(double scale);
		void setRotatingSpreadAttackSettings(double duration, double interval);
		void setTargetAttackSettings(double duration, double interval);
		void setTargetAttackSpeedSettings(double baseSpeed, double intervalSpeed);
		void setLineAttackSettings(double moveDuration, double targetY, double cherryInterval);
		void setLineAttackWarningSettings(double warnScale, double scaleUpTime, double scaleDownTime, double generateWaitTime);
		void barrageUpdate() override;
		void draw() const override;
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
		String customTextureName = U"";
		int32 customTextureEdge = 32;
		bool customTextureAnimation = false;
		ColorF customTextureColor{ 1.0, 1.0, 1.0, 1.0 };
	public:
		BossBarrageCherry(Vec2 startPos, double scale, BossCherryType cType);

		void barrageUpdate() override;
		void draw() const override;

		void setCustomTexture(String textureName, int32 textureEdge = 32, bool hasAnimation = false);
		void setCustomTextureColor(ColorF color);
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

	class TayamaLineCherry : public BossBarrageCherry {
	private:
		Vec2 startPos;
		Vec2 targetPos;
		double moveDuration = 1.0;
		double moveElapsed = 0.0;
		double fallSpeed = 0.0;
		double fallAcceleration = 0.35;
		int32 attackStep = 0;

	public:
		TayamaLineCherry(Vec2 startPos, Vec2 targetPos, double scale, double duration);

		void barrageUpdate() override;
	};

	class TayamaSecondPhaseEyeCherry : public BossBarrageCherry {
	private:
		double acceleration = 0.0;

	public:
		TayamaSecondPhaseEyeCherry(Vec2 startPos, double scale, double initialSpeed, double acceleration);

		void barrageUpdate() override;
	};

	class TayamaSecondPhaseTargetCherry : public BossBarrageCherry {
	private:
		BossStageManager* bossStageManager = nullptr;
		Stopwatch moveStopwatch{ StartImmediately::No };
		double moveSpeed = 4.0;
		double moveDuration = 0.7;
		double stopDuration = 0.35;
		int32 moveCount = 0;
		int32 maxMoveCount = 3;
		bool isStopping = false;

		void aimAtPlayer();

	public:
		TayamaSecondPhaseTargetCherry(Vec2 startPos, double scale, double speed, double moveTime, double stopTime, int32 moveNum, BossStageManager& manager);

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
