#pragma once
#include <Siv3D.hpp>
#include "../GameObject/StockNearGameObjects.h"
#include "../GameObject/Player.h"
#include "../GameObject/Bullet.h"
#include "../GameObject/Cherry.h"
#include "../GameObject/Boss/BossCherry.h"
#include "../GameObject/Boss/ExBossCherry.h"
#include "../GameObject/Block.h"
#include "../GameObject/Spike.h"
#include "../GameObject/SavePoint.h"
#include "../GameObject/Blood.h"
#include "../GameObject/Warp.h"
#include "../UI/TitleCard.h"
#include "CameraShake.h"
#include "../Global.h"


namespace Iwanna {

	struct BossStageGameObjects {
		std::shared_ptr<Player> player;
		Array<std::shared_ptr<Bullet>> bullets;
		Array<std::shared_ptr<Cherry>> cherries;
		Array<std::shared_ptr<Cherry>> bossCherries;
		Array<std::shared_ptr<Block>> blocks;
		Array<std::shared_ptr<Spike>> spikes;
		Array<std::shared_ptr<BossSavePoint>> savePoints;
		Array<std::shared_ptr<Blood>> bloods;
		Array<std::shared_ptr<Warp>> warps;
	};

	class BossStageManager {
	private:
		StockNearGameObjects stockNearGameObjects;
		StockNearGameObjects stockBulletsNearGameObjects;
		StockNearGameObjects stockLargeNearGameObjects;//大きいオブジェクトなど、通常のストッククラスでは処理できないものを入れる用
		BossStageGameObjects gameObjects;

		// 追加するりんごを一時格納するためのもの
		Array<std::shared_ptr<Cherry>> pendingCherries;

		//カメラ関連
		Vec2 cameraBasePos{ 400, 304 };
		Camera2D camera{ cameraBasePos, 1.0,CameraControl::None_ };
		double cameraScale = 1.0;
		CameraShake cameraShake;

		//タイトルカード(画面右上に表示するやつ)
		TitleCard titleCard;

		//背景用
		String backgroundName;

		//弾丸関連
		double bulletSpeed = 12;
		int32 bulletMaxNum = 5;

		//血しぶき数
		int32 bloodNum = 80;
		//血を生成したかどうか
		bool isGenerateBloods = false;

		//ステージ情報
		int32 oneTileSize = 32;
		String stageName;

		//GAMEOVER画面用
		Timer gameoverTimer{ 0.5s };
		bool isShowGameOver = false;

		int32 step = 0;

		// boss関連
		int32 defeatedBossNum = 0;
		bool isTrapBossSecondPhaseIntroStarted = false;
		bool isTrapBossSecondPhaseStarted = false;
		bool isTrapBossSecondPhaseDefeated = false;
		Stopwatch trapBossSecondPhaseIntroStopwatch{ StartImmediately::No };
		double trapBossSecondPhaseIntroCooldown = 4.0;
		double trapBossSecondPhaseIntroTime = 1.0;
		Vec2 trapBossSecondPhaseTayamaStartPos{ 316, 800 };
		Vec2 trapBossSecondPhaseTayamaEndPos{ 316, 260 };
		Vec2 trapBossSecondPhaseTayamaCenterPos{ 370, 304 };
		double trapBossSecondPhaseTayamaScale = 1.0;
		int32 trapBossSecondPhaseMaxHp = 30;
		int32 trapBossSecondPhaseHp = 30;
		double trapBossSecondPhaseEyeHitRadius = 24.0;
		Vec2 trapBossSecondPhaseLeftEyeOffset{ -22, 5 };
		Vec2 trapBossSecondPhaseRightEyeOffset{ 140, 15 };
		bool isTrapBossSecondPhaseEyeHitFlash = false;
		Stopwatch trapBossSecondPhaseEyeHitFlashStopwatch{ StartImmediately::No };
		double trapBossSecondPhaseEyeHitFlashTime = 0.18;
		double trapBossSecondPhaseEyeHitFlashAlpha = 0.35;

		//暗転演出関連
		double darkAlpha = 0.8;
		Timer darkAlphaTimer{0.5s,StartImmediately::Yes};

		//player hp UI関連
		Vec2 playerHpBasePos{0,576};
		double hpInterbalX = 32;

	public:
		BossStageManager();

		void setUpObjects(int32 chapter);
		void loadGameObjects(String);
		Vec2 parsePos(const JSON& json);
		Vec2 parseIntactPos(const JSON& json);

		void update();
		void debug();
		void draw();
		void updateTrapBossSecondPhaseIntro();
		void updateTrapBossSecondPhaseBulletHits(Array<std::shared_ptr<Bullet>>& bullets);
		void drawTrapBossSecondPhaseIntro() const;
		void drawTrapBossSecondPhaseTayama() const;
		void drawTrapBossSecondPhaseEyeHitBoxes() const;
		void drawTrapBossSecondPhaseHp() const;
		void hitTrapBossSecondPhase();
		void defeatTrapBossSecondPhase();
		void setStep(int32 newStep);
		void saveGame();
		Vec2 executeCameraPos();

		void generateBoss(int32 type);

		bool bossBgmStart = false;

		//取得用関数
		std::shared_ptr<Player> getPlayer();
		Array<std::shared_ptr<Cherry>> getCherries();
		Array<std::shared_ptr<Block>> getBlocks();
		CameraShake& getCameraShake() { return cameraShake; }
		Vec2 getTrapBossSecondPhaseLeftEyePos() const;
		Vec2 getTrapBossSecondPhaseRightEyePos() const;
		Array<Vec2> getTrapBossSecondPhaseEyePositions() const;

		//ExBoss用の取得関数
		std::shared_ptr<SordCherriesManager> getExBossSordManagerCherry();

		String getStageName() const;

		void createCherry(std::shared_ptr<Cherry> cherry);

		void createPeripheryBlocks();
		void createFloorBlocks(Vec2 basePos);

		// --- 計算関数 --- //
		//2つの座標から角度を計算
		double calculateDirection(Vec2 basePos, Vec2 targetPos) {
			Vec2 diff = targetPos - basePos;
			return Math::ToDegrees(Atan2(-diff.y, diff.x));
		}

		//2つの座標から距離を計算
		double calculateDistance(Vec2 basePos, Vec2 targetPos) {
			return basePos.distanceFrom(targetPos);
		}

		// りんご生成パターン(別cppファイルで定義)
		void createCherrySpread(int32 num, double spd, const std::function<std::shared_ptr<Cherry>()>& factory);
		void createSubThrowCherry(double dir, double spd, const std::function<std::shared_ptr<Cherry>()>& factory);
		void createBlueLineCherry(int32 num, double interval, const std::function<std::shared_ptr<Cherry>()>& factory);
		void createYellowStarCherry(int32 Nkakkei, int32 nextNumber, Vec2 center, int32 lineNum, const std::function<std::shared_ptr<Cherry>()>& factory);
		void createGreenWaveCherry(Vec2 startPos, double interval, double high, const std::function<std::shared_ptr<BossGreenWaveCherry>()>& factory);
		void createOrangeStopCherry(bool isAddUpDown, const std::function<std::shared_ptr<BossOrangeStopCherry>()>& factory);
		void createSkyTargetCherry(int32 lineNum, bool isAddLine, const std::function<std::shared_ptr<BossSkyTargetCherry>()>& factory);
		void createGrayLatticeCherry(double interval,const std::function<std::shared_ptr<BossGrayLatticeCherry>()>& factory);

		// ----- ExBoss用 ----- //
		//void createSordCherry(Vec2 startPos, const std::function<std::shared_ptr<BossSordCherry>()>& factory);
		void createSordExproCherry(Vec2 startPos, bool isEffect, const std::function<std::shared_ptr<Cherry>()>& factory);
		void createWarpCurtainCherry(Vec2 basePos, const std::function<std::shared_ptr<WarpCurtainCherry>()>& factory);
		void createSordFallSwingShockWaveCherry(Vec2 startPos,double baseDir, const std::function<std::shared_ptr<Cherry>()>& factory);
	};
}
