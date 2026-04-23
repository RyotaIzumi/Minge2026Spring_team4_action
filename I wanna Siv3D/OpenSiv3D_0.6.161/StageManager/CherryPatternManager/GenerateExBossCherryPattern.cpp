#include "../BossStageManager.h"

namespace Iwanna {
	/**
		 * @brief 剣のきらめきから繋がる爆破攻撃
		 * @param startPos 開始地点
		 * @param isEffect  
		 * @param cherry 生成するcherryオブジェクト
		 */
	void BossStageManager::createSordExproCherry(Vec2 startPos, bool isEffect, const std::function<std::shared_ptr<Cherry>()>& factory) {
		const int32 num = 30;
		const int32 onePointNum = 10;
		const double interval = 100;

		if (isEffect) {
			for (int i = 0; i < num; i++) {
				auto cherry = factory();
				cherry->pos.x = interval * i + startPos.x;
				cherry->pos.y = startPos.y;
				createCherry(cherry);
			}
		}
		else {
			for (int j = 0; j < onePointNum; j++) {
				for (int i = 0; i < num; i++) {
					auto cherry = factory();
					cherry->pos.x = interval * i + startPos.x;
					cherry->pos.y = startPos.y;
					cherry->direction = 85 + Random(10);
					cherry->speed = 20 + Random(5);
					createCherry(cherry);
				}
			}
			cameraShake.shake(0.7, 30.0);
		}
	}

	void BossStageManager::createWarpCurtainCherry(Vec2 basePos, const std::function<std::shared_ptr<WarpCurtainCherry>()>& factory) {
		const int32 numX = 9;
		const int32 numY = 20;
		const double interval = 32;

		Vec2 startPos = Vec2{ basePos.x - (interval * (numX - 1) / 2), 0 };
		for (int j = 0; j < numY; j++) {
			for (int i = 0; i < numX; i++) {
				auto cherry = factory();
				cherry->endPos.x = startPos.x + i * interval;
				cherry->endPos.y = startPos.y + j * interval;
				cherry->startPos.x = basePos.x;
				cherry->startPos.y = cherry->endPos.y;
				createCherry(cherry);
			}
		}
	}
}
