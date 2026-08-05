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
#include "../GameObject/Item.h"
#include "../UI/TitleCard.h"
#include "../UI/Achieve.h"
#include "../UI/BossHpBar.h"
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
		Array<std::shared_ptr<Item>> items;
	};

	class BossStageManager {
	private:
		StockNearGameObjects stockNearGameObjects;
		StockNearGameObjects stockBulletsNearGameObjects;
		StockNearGameObjects stockLargeNearGameObjects;//大きいオブジェクトなど、通常のストッククラスでは処理できないものを入れる用
		BossStageGameObjects gameObjects;

		// 追加するりんごを一時格納するためのもの
		Array<std::shared_ptr<Cherry>> pendingCherries;
		Array<std::shared_ptr<Cherry>> pendingBossCherries;

		//カメラ関連
		Vec2 cameraBasePos{ 400, 304 };
		Camera2D camera{ cameraBasePos, 1.0,CameraControl::None_ };
		double cameraScale = 1.0;
		CameraShake cameraShake;

		//タイトルカード(画面右上に表示するやつ)
		TitleCard titleCard;
		Achieve achive;

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
		bool shouldCleanupBossCherryDefeatObjects = false;
		bool isTrapBossSecondPhaseIntroStarted = false;
		bool isTrapBossSecondPhaseStarted = false;
		bool isTrapBossSecondPhaseDefeated = false;
		bool isTrapBossSecondPhaseDefeatedFall = false;
		bool hasTrapBossSecondPhaseBrokenBlocks = false;
		Stopwatch trapBossSecondPhaseIntroStopwatch{ StartImmediately::No };
		double trapBossSecondPhaseIntroCooldown = 4.0;
		double trapBossSecondPhaseIntroTime = 1.0;
		Vec2 trapBossSecondPhaseTayamaStartPos{ 316, 800 };
		Vec2 trapBossSecondPhaseTayamaEndPos{ 316, 260 };
		Vec2 trapBossSecondPhaseTayamaCenterPos{ 370, 304 };
		double trapBossSecondPhaseTayamaScale = 1.0;
		double trapBossSecondPhaseTayamaAngle = 0.0;
		double trapBossSecondPhaseDefeatedFallSpeed = 0.0;
		double trapBossSecondPhaseDefeatedFallAcceleration = 0.25;
		double trapBossSecondPhaseDefeatedRotateSpeed = 90.0;
		int32 trapBossSecondPhaseMaxHp = 22;
		int32 trapBossSecondPhaseHp = 22;
		BossHpBarDelayState trapBossSecondPhaseHpBarDelay;
		double trapBossSecondPhaseEyeHitRadius = 24.0;
		Vec2 trapBossSecondPhaseLeftEyeOffset{ -22, 5 };
		Vec2 trapBossSecondPhaseRightEyeOffset{ 140, 15 };
		bool isTrapBossSecondPhaseEyeHitFlash = false;
		Stopwatch trapBossSecondPhaseEyeHitFlashStopwatch{ StartImmediately::No };
		double trapBossSecondPhaseEyeHitFlashTime = 0.18;
		double trapBossSecondPhaseEyeHitFlashAlpha = 0.35;
		int32 trapBossSecondPhaseBreakBlockRange = 4;
		double trapBossSecondPhaseStartShakeTime = 2.0;
		double trapBossSecondPhaseStartShakePower = 90.0;
		Stopwatch trapBossSecondPhaseAttackStopwatch{ StartImmediately::No };
		int32 trapBossSecondPhaseEyeAttackCount = 0;
		double trapBossSecondPhaseEyeAttackStartDelay = 2.0;
		double trapBossSecondPhaseEyeAttackInterval = 0.2;
		double trapBossSecondPhaseEyeAttackBaseDirection = 90.0;
		double trapBossSecondPhaseEyeAttackAngleStep = 30.0;
		double trapBossSecondPhaseEyeAttackCherrySpeed = 3.0;
		double trapBossSecondPhaseEyeAttackCherryAcceleration = 0.08;
		double trapBossSecondPhaseEyeAttackCherryScale = 1.2;
		Stopwatch trapBossSecondPhaseTargetAttackStopwatch{ StartImmediately::No };
		int32 trapBossSecondPhaseTargetAttackCount = 0;
		double trapBossSecondPhaseTargetAttackStartDelay = 3.0;
		double trapBossSecondPhaseTargetAttackInterval = 4.0;
		double trapBossSecondPhaseTargetAttackCherrySpeed = 4.5;
		double trapBossSecondPhaseTargetAttackCherryScale = 1.5;
		double trapBossSecondPhaseTargetAttackMoveTime = 1.0;
		double trapBossSecondPhaseTargetAttackStopTime = 0.5;
		int32 trapBossSecondPhaseTargetAttackMoveCount = 3;

		//暗転演出関連
		double darkAlpha = 0.8;
		Timer darkAlphaTimer{0.5s,StartImmediately::Yes};
		double trapBossSecondPhaseDarkAlpha = 0.15;
		double trapBossSecondPhaseDarkAlphaMin = 0.10;
		double trapBossSecondPhaseDarkAlphaMax = 0.25;
		double trapBossSecondPhaseDarkAlphaFadeSpeed = 0.08;
		bool isExBossDarkEffectActive = false;
		double exBossDarkAlpha = 0.0;
		double exBossDarkAlphaMin = 0.05;
		double exBossDarkAlphaMax = 0.35;
		double exBossDarkAlphaFadeSpeed = 0.05;
		double exBossEntryDarkAlpha = 0.0;
		double exBossEntryDarkAlphaFadeSpeed = 0.03;
		bool isExBossThirdPhaseDarkening = false;
		bool isExBossThirdPhaseRestoring = false;
		double exBossThirdPhaseDarkAlphaTarget = 0.9;
		double exBossThirdPhaseDarkAlphaSpeed = 0.01;
		double exBossThirdPhaseRestoreSpeed = 0.01;
		bool isExBossCameraLocked = false;
		Vec2 exBossLockedCameraCenter{ 800, 304 };

		// 召喚LowBoss
		bool hasExBossThirdPhaseLowBoss = false;
		double exBossLowBossScale = 5.0;
		double exBossLowBossAppearDuration = 1.4;
		double exBossLowBossTargetY = 304.0;
		double exBossLowBossLifeTime = 15.0;
		double exBossLowBossSpreadInterval = 0.6;
		int32 exBossLowBossSpreadCherryNum = 16;
		double exBossLowBossSpreadCherrySpeed = 5.0;
		double exBossLowBossTargetInterval = 1.6;
		int32 exBossLowBossTargetLineNum = 3;
		bool exBossLowBossTargetIsAddLine = false;
		double exBossLowBossTargetBaseSpeed = 4.0;
		double exBossLowBossTargetIntervalSpeed = 1.0;

		// 召喚BossCherry
		bool hasExBossThirdPhaseBossCherry = false;
		double exBossBossCherryScale = 5.0;
		double exBossBossCherryFunnelScale = 2.0;
		double exBossBossCherryAppearDuration = 1.4;
		double exBossBossCherryTargetY = 304.0;
		double exBossBossCherryLifeTime = 16.0;
		double exBossBossCherryAttackInterval = 1.2;

		// 召喚Tayama第二形態
		bool hasExBossThirdPhaseTayama = false;
		bool isExBossThirdPhaseTayamaLeaving = false;
		int32 exBossThirdPhaseTayamaStep = 0;
		Vec2 exBossThirdPhaseTayamaCenterPos{ 800, 304 };
		Vec2 exBossThirdPhaseTayamaStartPos{ 800, -320 };
		Vec2 exBossThirdPhaseTayamaTargetPos{ 800, 304 };
		Stopwatch exBossThirdPhaseTayamaStopwatch{ StartImmediately::No };
		Stopwatch exBossThirdPhaseTayamaAttackStopwatch{ StartImmediately::No };
		Stopwatch exBossThirdPhaseTayamaTargetAttackStopwatch{ StartImmediately::No };
		int32 exBossThirdPhaseTayamaEyeAttackCount = 0;
		int32 exBossThirdPhaseTayamaTargetAttackCount = 0;
		double exBossThirdPhaseTayamaScale = 1.0;
		double exBossThirdPhaseTayamaAppearDuration = 1.4;
		double exBossThirdPhaseTayamaTargetY = 304.0;
		double exBossThirdPhaseTayamaLifeTime = 16.0;
		double exBossThirdPhaseTayamaLeaveVelocity = 0.0;
		double exBossThirdPhaseTayamaLeaveSpeed = 2.5;
		double exBossThirdPhaseTayamaLeaveAcceleration = 0.08;

		// 召喚ボス名前表示
		bool isExBossSummonNameBarWaiting = false;
		bool isExBossSummonNameBarActive = false;
		String exBossSummonNameBarTextureName = U"";
		String exBossSummonNameBarWaitingTextureName = U"";
		Vec2 exBossSummonNameBarCenterPos{ 0, 0 };
		Vec2 exBossSummonNameBarWaitingCenterPos{ 0, 0 };
		Vec2 exBossSummonNameBarAnchor{ 12, 73 };
		Stopwatch exBossSummonNameBarStopwatch{ StartImmediately::No };
		Stopwatch exBossSummonNameBarWaitingStopwatch{ StartImmediately::No };
		double exBossSummonNameBarWaitingDelay = 0.0;
		double exBossSummonNameBarFadeInTime = 0.35;
		double exBossSummonNameBarShowTime = 2.5;
		double exBossSummonNameBarFadeOutTime = 0.5;

		Stopwatch trapBossGuygunStopwatch{ StartImmediately::Yes };
		double trapBossGuygunInterval = 0.06;
		double trapBossGuygunVolume = 0.3;

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
		void updateTrapBossSecondPhaseDefeatedFall();
		void updateTrapBossSecondPhaseEyeAttack();
		void updateTrapBossSecondPhaseTargetAttack();
		void updateExBossThirdPhaseTayamaSummon();
		void updateExBossSummonNameBar();
		void updateItem2Pickup();
		void updateBulletSpikeHits();
		void updateTrapBossSecondPhaseBulletHits(Array<std::shared_ptr<Bullet>>& bullets);
		void breakTrapBossSecondPhaseOverlappingBlocks();
		void createTrapBossSecondPhaseEyeAttackCherry(Vec2 startPos, double direction);
		void createTrapBossSecondPhaseTargetAttackCherry(Vec2 startPos);
		void drawTrapBossSecondPhaseIntro() const;
		void drawTrapBossSecondPhaseTayama() const;
		void drawTrapBossSecondPhaseEyeHitBoxes() const;
		void drawExBossThirdPhaseTayama() const;
		void drawExBossSummonNameBar() const;
		void drawTrapBossSecondPhaseHp() const;
		void hitTrapBossSecondPhase(int32 damage = 1);
		void defeatTrapBossSecondPhase();
		void clearTrapBossSecondPhaseCherries();
		void requestBossCherryDefeatCleanup();
		void cleanupBossCherryDefeatObjects();
		void spawnItem2();
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
		bool shouldStopBossBgm() const;
		bool isTrapBossSecondPhaseBgm() const;
		void startExBossThirdPhaseDarkening();
		bool isExBossThirdPhaseDarkened() const;
		void summonExBossThirdPhaseLowBoss();
		bool isExBossThirdPhaseLowBossFinished() const;
		void summonExBossThirdPhaseBossCherry();
		bool isExBossThirdPhaseBossCherryFinished();
		void summonExBossThirdPhaseTayama();
		bool isExBossThirdPhaseTayamaFinished() const;
		void reserveExBossSummonNameBar(String textureName, Vec2 centerPos, double delay);
		void startExBossSummonNameBar(String textureName, Vec2 centerPos);
		void finishExBossThirdPhaseLowBoss();
		Vec2 getExBossLockedCameraCenter() const;
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
		void createSkyTargetCherry(int32 lineNum, bool isAddLine, const std::function<std::shared_ptr<BossSkyTargetCherry>()>& factory, double baseSpd = 5.0, double interSpd = 2.0);
		void createGrayLatticeCherry(double interval,const std::function<std::shared_ptr<BossGrayLatticeCherry>()>& factory);

		// ----- ExBoss用 ----- //
		//void createSordCherry(Vec2 startPos, const std::function<std::shared_ptr<BossSordCherry>()>& factory);
		void createSordExproCherry(Vec2 startPos, bool isEffect, const std::function<std::shared_ptr<Cherry>()>& factory);
		void createWarpCurtainCherry(Vec2 basePos, const std::function<std::shared_ptr<WarpCurtainCherry>()>& factory);
		void createSordFallSwingShockWaveCherry(Vec2 startPos,double baseDir, const std::function<std::shared_ptr<Cherry>()>& factory);
	};
}
