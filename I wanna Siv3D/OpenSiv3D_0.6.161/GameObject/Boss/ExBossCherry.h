#pragma once
#include "../Cherry.h"

namespace Iwanna {
	class BossStageManager;

	enum class SordCherryType {
		Blade,
		Handle
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
	public:
		SordCherriesManager(Vec2 startPos, double scale, BossStageManager& manager);
		void barrageUpdate() override;
		void draw() const override;

		//設定関数
		void setSordBaseCenterPos(Vec2 pos);
		void setExBossPos(Vec2 pos);
		void setExBossAngle(double);

		//状態設定関数
		void createSordCherries(); //剣型の生成
		void startFollowBoss(); //bossの移動についていくようにする関数
		void sparkSordBlade(); // 剣を光らせる
	};

	enum class ExBossAttackType {
		None,
		SparkExpro
	};

	class ExBossCherry : public Cherry {
	private:
		Vec2 baseCenterPos;
		double c, r;
		bool isFollowBoss = false;

		ExBossAttackType nowAttackType = ExBossAttackType::None;
		int32 attackStep = 0;

		Stopwatch attackStopwatch{ StartImmediately::No };
		double attackIntervalTime;

		Timer startAttackTimer{ 0.8s, StartImmediately::Yes };

		//攻撃関連関数
		Vec2 attackStartPos;

	protected:
		BossStageManager* bossStageManager = nullptr;
		SordCherriesManager* sordCherriesManager = nullptr;
		int32 startStep = 0;
	public:
		ExBossCherry(Vec2 startPos, double scale, BossStageManager& manager);

		void barrageUpdate() override;
		void attack();
		void draw() const override;
	};

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

	class ExproCherry : public Cherry {
	private:
		Timer attackTimer{ 0.4s,StartImmediately::No };
		double startSpeed = 0.0;
		int32 step = 0;
	public:
		ExproCherry(Vec2 startPos, double scale);
		void barrageUpdate() override;
	};
}
