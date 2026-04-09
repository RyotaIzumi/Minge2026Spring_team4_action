#pragma once
#include <Siv3D.hpp>
#include "../GameObject/StockNearGameObjects.h"
#include "../GameObject/Player.h"
#include "../GameObject/Bullet.h"
#include "../GameObject/Cherry.h"
#include "../GameObject/Boss/BossCherry.h"
#include "../GameObject/Block.h"
#include "../GameObject/Spike.h"
#include "../GameObject/SavePoint.h"
#include "../GameObject/Blood.h"
#include "../GameObject/Warp.h"
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

	//カメラの揺れを管理する構造体
	struct CameraShake
	{
		double time = 0.0;
		double power = 0.0;
		double frequency = 30;

		void shake(double t = 0.5, double p = 10.0)
		{
			time = t;
			power = p;
		}

		void update()
		{
			if (time > 0.0)	time -= Scene::DeltaTime();
		}

		Vec2 getOffset() const
		{
			if (time <= 0.0)
			{
				return Vec2{ 0, 0 };
			}
			double t = Scene::Time();
			// 縦方向のみ
			double y = Math::Sin(t * frequency) * power * time;
			return Vec2{ 0, y };
		}
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
		Camera2D camera{ cameraBasePos, 1.0};
		double cameraScale = 1.0;
		CameraShake cameraShake;

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

		//暗転演出関連
		double darkAlpha = 0.8;
		Timer darkAlphaTimer{2.1s,StartImmediately::Yes};

	public:
		BossStageManager();

		void setUpObjects(int32 chapter);
		void loadGameObjects(String);
		Vec2 parsePos(const JSON& json);
		Vec2 parseIntactPos(const JSON& json);

		void update();
		void debug();
		void draw();
		void setStep(int32 newStep);
		void saveGame();
		Vec2 executeCameraPos();

		void generateBoss(int32 type);

		bool bossBgmStart = false;

		//取得用関数
		std::shared_ptr<Player> getPlayer();
		Array<std::shared_ptr<Cherry>> getCherries();
		Array<std::shared_ptr<Block>> getBlocks();

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
	};
}
