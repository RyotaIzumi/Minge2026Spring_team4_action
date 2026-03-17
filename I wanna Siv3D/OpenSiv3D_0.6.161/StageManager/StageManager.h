#pragma once
#include <Siv3D.hpp>
#include "../GameObject/StockNearGameObjects.h"
#include "../GameObject/Player.h"
#include "../GameObject/Bullet.h"
#include "../GameObject/Cherry.h"
#include "../GameObject/Block.h"
#include "../GameObject/Spike.h"
#include "../GameObject/Trigger.h"
#include "../GameObject/SavePoint.h"
#include "../GameObject/SpecialTrap.h"
#include "../GameObject/Blood.h"
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
	};

	class StageManager {
	private:
		StockNearGameObjects stockNearGameObjects;
		StockNearGameObjects stockBulletsNearGameObjects;
		StockNearGameObjects stockLargeNearGameObjects;//大きいオブジェクトなど、通常のストッククラスでは処理できないものを入れる用
		StageGameObjects gameObjects;

		//カメラ関連
		Vec2 cameraBasePos{ 400, 304 };
		Camera2D camera{ cameraBasePos, 1.0 };

		//弾丸関連
		double bulletSpeed = 12;
		int32 bulletMaxNum = 5;

		//血しぶき数
		int32 bloodNum = 80;
		//血を生成したかどうか
		bool isGenerateBloods = false;

		int32 oneTileSize = 32;

		int32 step = 0;
	public:
		StageManager();

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

		std::shared_ptr<Player> getPlayer();
		Array<std::shared_ptr<Cherry>> getCherries();
		Array<std::shared_ptr<Block>> getBlocks();

		//ある罠用に取得用
		std::shared_ptr<SpecialTrap> getWarningWindowTrap();

		void createCherry(std::shared_ptr<Cherry> cherry);

		void createPeripheryBlocks();
		void createFloorBlocks(Vec2 basePos);
	};
}
