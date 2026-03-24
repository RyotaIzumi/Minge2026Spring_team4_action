#include "../BossStageManager.h"

namespace Iwanna {
	// 汎用性の高い生成パターン関数を集めたファイル

	/**
		 * @brief りんご拡散弾生成
		 * @param num 生成数
		 * @param spd 速度
		 * @param cherry 生成するcherryオブジェクト
		 */
	void BossStageManager::createCherrySpread(int32 num, double spd, const std::function<std::shared_ptr<Cherry>()>& factory) {
		const double dirRange = 360.0;
		const double startDir = Random(360);

		for (int i = 0; i < num; i++) {
			auto cherry = factory();
			cherry->speed = spd;
			cherry->direction = i * (dirRange / num) + startDir;
			createCherry(cherry);
		}
	}

	/**
		 * @brief ボスの投げるサブりんご生成
		 * @param dir 角度
		 * @param spd 速度
		 * @param cherry 生成するcherryオブジェクト
		 */
	void BossStageManager::createSubThrowCherry(double dir, double spd, const std::function<std::shared_ptr<Cherry>()>& factory) {

		auto cherry = factory();
		cherry->speed = spd;
		cherry->direction = dir;
		createCherry(cherry);
	}

	/**
		 * @brief 青攻撃用のライン型
		 * @param num 生成数
		 * @param interval 間隔
		 * @param cherry 生成するcherryオブジェクト
		 */
	void BossStageManager::createBlueLineCherry(int32 num, double interval, const std::function<std::shared_ptr<Cherry>()>& factory) {
		const double startX = -1 * Random(interval);
		for (int i = 0; i < num; i++) {
			auto cherry = factory();
			cherry->pos.x = startX + interval * i;
			cherry->pos.y = -100;
			createCherry(cherry);
		}
	}
}
