#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Sprite/SpriteSystem.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	class StageManager;
	// りんごの種類
	enum class CherryType {
		Normal,
		Boss,
		BossSub,
		Barrage,
		Trap,
		Gimmik
	};
	// りんごの色の種類
	enum class CherryColorType {
		None,
		Red,
		Blue,
		Yellow,
		Green,
		Orange,
		Sky
	};
	class Cherry : public GameObject {
	private:
		//movePosition用
		Vec2 targetPos, startPos;
		bool isMoving = false;
		bool isAccelerationMode = false;
		double moveDuration = 0.0; // 移動にかける時間（秒）
		double moveElapsed = 0.0; // 経過時間
		double easing = 0.1; // 0.05～0.2くらい
	protected:
		//当たり判定サイズ(半径)
		int32 hitBoxSize = 10;
		//大きさの倍率
		double scaleMag = 1.0;
		//画面外に出たら削除するか
		bool isDeleteOutOfScreen = true;
		//自身の種類
		CherryType cherryType = CherryType::Normal;

		//hpを持ち、倒せるかどうか
		bool hasHp = false;
		int32 maxHp;
		int32 hp;
		bool isMuteki = false;
		Timer mutekiInterval{0.5s};

		//描画,アニメーション関連
		bool hasAnimation = false;
		int32 textureEdge = 32;
		String cherryTextureName = U"sprCherryLow";

		CherryColorType cherryColorType;
		ColorF typeColor;

	public:
		bool isDelete = false;//消去用フラグ
		bool isOutOfScreen = false;//画面外判定用フラグ

		Cherry(Vec2 startPos, double scale = 1.0);

		void update() override;
		virtual void barrageUpdate();
		virtual void trapUpdate(int32 id);

		void setTypeColor();
		void draw() const override;

		void movePosition(const Vec2& targetPoint, double timeSec, bool accele = false);
		void updateForMoveTargetPos();
		bool getIsMoveFinished() const;

		void checkOutOfScreen();

		bool getHasHp() const;
		void hited();
		bool getIsMuteki() const;

		CherryType getCherryType() const;

		void onCollision(GameObject& other) override;
	};

	class SpriteCherry : public Cherry {
	public:
		SpriteCherry(String name, Vec2 startPos, double scale);
	};

	class CherryTrap : public Cherry {
	private:
		int32 trapID = 0;
		int32 trapSpeed;
		bool isTrapActived = false;
		bool soundPlayOne = false;
	public:
		CherryTrap(Vec2 startPos, int32 id, double direction, double speed);
		void trapUpdate(int32 id) override;

		int32 getTrapID() const;
	};

	class BarrageCherry : public Cherry {
	protected:
		int32 startStep = 0;
	public:
		BarrageCherry(Vec2 startPos, double scale, CherryColorType colorType);

		void barrageUpdate() override;
	};

	class BarrageGimmikBlueCherry : public BarrageCherry {
	public:
		BarrageGimmikBlueCherry(Vec2 startPos, double scale, CherryColorType colorType);

		void barrageUpdate() override;
	};

	class BarrageGimmikYellowCherry : public BarrageCherry {
	private:
		Vec2 centerPos;
		double r, c;
	public:
		BarrageGimmikYellowCherry(Vec2 startPos, double scale, CherryColorType colorType);

		void barrageUpdate() override;
	};

	class GimmikBigCherry : public Cherry {
	private:
		StageManager* stageManager = nullptr;

		Timer startTimer{ 2.2s, StartImmediately::Yes };
		Stopwatch attackIntervalStopwatch{StartImmediately::No};
		double attackInterval = 0;
	public:
		GimmikBigCherry(Vec2 startPos, double scale, CherryColorType cType, StageManager& manager);

		void barrageUpdate() override;

		void generateAttack();
	};
}
