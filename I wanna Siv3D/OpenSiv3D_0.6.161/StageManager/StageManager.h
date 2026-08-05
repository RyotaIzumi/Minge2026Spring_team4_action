#pragma once
#include <Siv3D.hpp>
#include "../GameObject/StockNearGameObjects.h"
#include "../GameObject/Player.h"
#include "../GameObject/Bullet.h"
#include "../GameObject/Cherry.h"
#include "../GameObject/Boss/BossCherry.h"
#include "../GameObject/Block.h"
#include "../GameObject/Spike.h"
#include "../GameObject/Trigger.h"
#include "../GameObject/SavePoint.h"
#include "../GameObject/SpecialTrap.h"
#include "../GameObject/Blood.h"
#include "../GameObject/Warp.h"
#include "../GameObject/Sign.h"
#include "../GameObject/Item.h"
#include "../UI/TitleCard.h"
#include "../UI/Achieve.h"
#include "CameraShake.h"
#include "../Global.h"

namespace Iwanna {

	struct StageGameObjects {
		std::shared_ptr<Player> player;
		Array<std::shared_ptr<Bullet>> bullets;
		Array<std::shared_ptr<Cherry>> cherries;
		Array<std::shared_ptr<Block>> blocks;
		Array<std::shared_ptr<Spike>> spikes;
		Array<std::shared_ptr<Trigger>> triggers;
		Array<std::shared_ptr<SavePoint>> savePoints;
		Array<std::shared_ptr<SpecialTrap>> specialTraps;
		Array<std::shared_ptr<SpecialTrap>> specialBackTraps;//↑と違い、背景側のレイヤー配置用
		Array<std::shared_ptr<Blood>> bloods;
		Array<std::shared_ptr<Warp>> warps;
		Array<std::shared_ptr<Sign>> signs;
		Array<std::shared_ptr<Item>> items;
	};

	class StageManager {
	private:
		StockNearGameObjects stockNearGameObjects;
		StockNearGameObjects stockBulletsNearGameObjects;
		StockNearGameObjects stockLargeNearGameObjects;//大きいオブジェクトなど、通常のストッククラスでは処理できないものを入れる用
		StageGameObjects gameObjects;

		// 追加するりんごを一時格納するためのもの
		Array<std::shared_ptr<Cherry>> pendingCherries;

		//カメラ関連
		Vec2 cameraBasePos{ 400, 304 };
		Camera2D camera{ cameraBasePos, 1.0 ,CameraControl::None_ };
		double cameraScale = 1.0;
		CameraShake cameraShake;

		//特殊トラップ用
		Vec2 saveTrapCameraPos{0,0};
		int32 specialSaveTrapTriggerID = 50;

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
		int32 latestActivatedTriggerID = -1;
		HashSet<int32> playedTrapPonTriggerIDs;

		//GAMEOVER画面用
		Timer gameoverTimer{ 0.5s };
		bool isShowGameOver = false;

		//背景
		String backgroundName;

		//タイトルカード(画面右上に表示するやつ)
		TitleCard titleCard;

		//Achievement表示
		Achieve achive;

		//暗転演出関連
		double darkAlpha = 0.8;
		Timer darkAlphaTimer{ 0.6s,StartImmediately::Yes };
		Array<String> darkEffectStages{U"secret1"};
		bool hasSecretEntranceFlashShown = false;
		double secretEntranceFlashAlpha = 0.0;
		double secretEntranceFlashStartAlpha = 0.65;
		double secretEntranceFlashFadeSpeed = 0.06;

		int32 step = 0;
	public:
		StageManager();

		void setUpObjects(int32 chapter);
		void loadGameObjects(String); //別cppファイルのLoadGameObjectsにて定義
		Vec2 parsePos(const JSON& json);
		Vec2 parseIntactPos(const JSON& json);

		void update();
		void updateBulletSpikeHits();
		void debug();
		void draw()
			;
		void setStep(int32 newStep);
		void saveGame();
		Vec2 executeCameraPos();

		//取得用関数
		std::shared_ptr<Player> getPlayer();
		Array<std::shared_ptr<Cherry>> getCherries();
		Array<std::shared_ptr<Block>> getBlocks();

		String getStageName() const;

		//ある罠用に取得用
		std::shared_ptr<SpecialTrap> getWarningWindowTrap();

		void createCherry(std::shared_ptr<Cherry> cherry);
		void createPlayerWarpEffectCherries(Vec2 centerPos);

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
		void createBlueLineCherry(const std::function<std::shared_ptr<Cherry>()>& factory);
		void createYellowStarCherry(int32 Nkakkei, int32 nextNumber, Vec2 center, int32 lineNum, const std::function<std::shared_ptr<Cherry>()>& factory);
		void createGreenWaveCherry(Vec2 startPos, double interval, double high, const std::function<std::shared_ptr<BarrageGimmikGreenCherry>()>& factory);
		void createOrangeStopCherry(bool isAddUpDown, const std::function<std::shared_ptr<BarrageGimmikOrangeCherry>()>& factory);
		void createSkyTargetCherry(int32 lineNum, const std::function<std::shared_ptr<BarrageCherry>()>& factory);
		void createGrayLatticeCherry(double interval, const std::function<std::shared_ptr<BossGrayLatticeCherry>()>& factory);
	};
}
