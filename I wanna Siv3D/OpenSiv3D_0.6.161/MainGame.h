#pragma once
#include <Siv3D.hpp>
#include "GameObject/StockNearGameObjects.h"
#include "AvoidanceManager/AvoidanceManager.h"
#include "StageManager/StageManager.h"	
#include "StageManager/BossStageManager.h"	

namespace Iwanna {
	enum class StageType {
		Normal,
		Boss
	};

	class MainGame {
	private:
		StockNearGameObjects stockNearGameObjects;

		Audio audio;

		AvoidanceManager avoidanceManager;
		StageManager stageManager;
		BossStageManager bossStageManager;

		StageType stageType;

	public:
		MainGame();

		void startGame();
		void updateGame();
		void debugGame();
		void drawGame();

		void playBgm(String);
		void stopBgm();

		void pauseBgm();
	};
}
