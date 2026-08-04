#pragma once
#include "../Cherry.h"
#include "../../UI/BossHpBar.h"

namespace Iwanna {
	class BossStageManager;

	enum class SordCherryType {
		Blade,
		Handle,
		Hitbox
	};

	class SordCherry : public BarrageCherry {
	private:
		Vec2 sordBaseCenterPos;
		double r, c;
		bool isFollowBoss = false;

		double exBossAngle = 0.0;//ExBossのtexture角度
		double exBossAngleOffset = 140;

		//刃のきらめき関連
		double startSparkTime = 0.0;
		Stopwatch sparkStopwatch{ StartImmediately::No };
		bool canSpark = false;
		double sparkAlpha = 0.0;
		Timer sparkTimer{ 0.14s,StartImmediately::No };
		int32 sparkStep = 0;
	public:
		SordCherry(Vec2 startPos, double scale, CherryColorType colorType);

		SordCherryType sordCherryType;
		int32 bladeId = -1;

		void barrageUpdate() override;
		void draw() const override;

		void setSordBaseCenterPos(Vec2);
		void setSordRotateStatus(Vec2);
		void setExBossAngle(double);
		void setIsFollowBoss(bool);

		void initSparking(double time);
	};

	class SordHitBoxCherry : public SordCherry {
	private:
		Vec2 sordBaseCenterPos;
		double r, c;
		bool isFollowBoss = false;

		double exBossAngle = 0.0;//ExBossのtexture角度
		double exBossAngleOffset = 140;
	public:
		SordHitBoxCherry(Vec2 startPos, double scale, CherryColorType colorType);

		void draw() const override;
	};

	class SordCherriesManager : public Cherry {
	private:
		Array<std::shared_ptr<SordCherry>> sordCherries;

		BossStageManager* bossStageManager = nullptr;
		bool isFollowBoss = false;
		Vec2 exBossPos{0,0};
		Vec2 offsetBoss;//ExBossの位置とのOffset
		double exBossAngle = 0.0;//ExBossのtexture角度
		double exBossAngleOffset = 140;
		double r, c;

		Vec2 hitBoxSize;

		Vec2 sordEdgePos;

		Timer generateEffectTimer{ 0.001s,StartImmediately::No };

	public:
		SordCherriesManager(Vec2 startPos, double scale, BossStageManager& manager);
		void barrageUpdate() override;
		void draw() const override;

		//設定関数
		void setSordBaseCenterPos(Vec2 pos);
		void setExBossPos(Vec2 pos);
		void setExBossAngle(double);
		void setSordCanPlayerKill(bool bl);

		Vec2 getSordEdgePos();

		//状態設定関数
		void createSordCherries(); //剣型の生成
		void startFollowBoss(); //bossの移動についていくようにする関数
		void sparkSordBlade(); // 剣を光らせる
	};

	enum class ExBossAttackType {
		None,
		Wait,
		SparkExpro,
		SwingOne,
		SwingTwo,
		SwingThree,
		Fall,
		FallSwing,
		Slide,
		Warp,
		ThirdFormRetreat
	};

	class ExBossCherry : public Cherry {
	private:
		Vec2 baseCenterPos;
		double c, r;
		bool isFollowBoss = false;
		bool isNowAttacking = false;

		double baseY = 390;//基本位置となるY座標
		double baseAngle = 0.0;//基本の角度

		ExBossAttackType nowAttackType = ExBossAttackType::None;
		int32 attackStep = 0;

		//待機状態関連
		Stopwatch waitStopwatch{ StartImmediately::No };
		double waitTime;

		//攻撃関連関数
		Vec2 attackStartPos;

		//情報
		Vec2 playerPos;
		bool isPlayerInRightSide = false;
		double playerDistance = 0.0;

		double hpBarAlpha = 0.0;
		BossHpBarDelayState hpBarDelay;

		//ボス自身のステータス
		enum class BossForm {
			First,
			Second,
			Third,
			Forth
		};

		BossForm bossForm = BossForm::First;
		BossForm prevBossForm = BossForm::First;
		bool isThirdFormRetreatPending = false;
		bool isThirdFormRetreatFinished = false;
		Stopwatch thirdFormRetreatStopwatch{ StartImmediately::No };
		double thirdFormRetreatWaitTime = 1.4;
		double thirdFormDarkeningWaitTime = 3.0;

		Array<ExBossAttackType> canAttackTypes{ ExBossAttackType::SwingOne, ExBossAttackType::Fall, ExBossAttackType::Slide, ExBossAttackType::SparkExpro, ExBossAttackType::Warp };
		Array<CherryColorType> randomChoiceBarrageAttacks{ CherryColorType::Red,CherryColorType::Yellow };

		Timer generateBarrageCherryTimer{ 0.3s ,StartImmediately::No};
		CherryColorType continueGenerateColor;

	protected:
		BossStageManager* bossStageManager = nullptr;
		SordCherriesManager* sordCherriesManager = nullptr;
		int32 startStep = 0;
	public:
		ExBossCherry(Vec2 startPos, double scale, BossStageManager& manager);

		void barrageUpdate() override;
		void draw() const override;
		void drawHpBarScreen() const;

		void updateBossForm();
		bool getRandomChance(double p);

		void decideAttack();

		void hited() override;

		// ExBossAttack.cppで定義
		void attack();
		void barrageAttack(CherryColorType type);
		void slideBarrageAttack(CherryColorType type);
		void startWait();

		double getBaseAngleDiff() const;
	};

	// ----- 攻撃に用いるりんご ----- //

	class ExproCherry : public Cherry {
	private:
		Timer attackTimer{ 0.5s,StartImmediately::No };
		double startSpeed = 0.0;
		int32 step = 0;
	public:
		ExproCherry(Vec2 startPos, double scale);
		void barrageUpdate() override;
	};

	// ----- 攻撃のエフェクト用りんご ----- //

	class EffectCherrySpark : public Cherry {
	private:
		Timer sparkTimer{ 0.6s,StartImmediately::No };
		double sparkAngle = 0.0;
		double addAngle = 0.0;
		double startAddAngle = 0.0;
		double addScale = 0.0;
		double startAddScale = 0.0;
	public:
		EffectCherrySpark(Vec2 startPos, double scale);
		void barrageUpdate() override;
		void draw() const override;
	};

	class FadeCherry : public Cherry {
	private:
		Timer fadeTimer;
		double fadeTime;
	public:
		FadeCherry(Vec2 startPos, double scale,String name, CherryColorType colorType,double duration);
		void barrageUpdate() override;
	};

	class WarpCurtainCherry : public Cherry {
	private:
		Timer fadeTimer;
		Timer waitTimer;
		int32 effectStep = 0;
	public:
		Vec2 startPos, endPos;

		WarpCurtainCherry(double scale);
		void barrageUpdate() override;
	};
}
