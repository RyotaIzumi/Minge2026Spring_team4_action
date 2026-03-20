#pragma once
#include <Siv3D.hpp>
#include "../GameObject/StockNearGameObjects.h"
#include "../GameObject/Player.h"
#include "../GameObject/Bullet.h"
#include "../GameObject/Cherry.h"
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
		Array<std::shared_ptr<Block>> blocks;
		Array<std::shared_ptr<Spike>> spikes;
		Array<std::shared_ptr<SavePoint>> savePoints;
		Array<std::shared_ptr<Blood>> bloods;
		Array<std::shared_ptr<Warp>> warps;
	};

	class BossStageManager {
	private:
		StockNearGameObjects stockNearGameObjects;
		StockNearGameObjects stockBulletsNearGameObjects;
		StockNearGameObjects stockLargeNearGameObjects;//大きいオブジェクトなど、通常のストッククラスでは処理できないものを入れる用
		BossStageGameObjects gameObjects;

		//カメラ関連
		Vec2 cameraBasePos{ 400, 304 };
		Camera2D camera{ cameraBasePos, 1.0 };
		double cameraScale = 1.0;

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
	public:
		BossStageManager();

		void setUpObjects(int32 chapter);
		void loadGameObjects(String);
		Vec2 parsePos(const JSON& json);
		Vec2 parseIntactPos(const JSON& json);

		void update();
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

		void createCherry(std::shared_ptr<Cherry> cherry);

		void createPeripheryBlocks();
		void createFloorBlocks(Vec2 basePos);
	};
}
