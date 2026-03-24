#include "../AvoidanceManager.h"

namespace Iwanna {
	// 汎用性の高い生成パターン関数を集めたファイル

	/**
		 * @brief りんご拡散弾生成
		 * @param pos 生成位置
		 * @param num 生成数
		 * @param spd 速度
		 * @param cherry 生成するcherryオブジェクト
		 */
	void AvoidanceManager::createCherrySpread(int32 num, double spd, const std::function<std::shared_ptr<Cherry>()>& factory){
		const double dirRange = 360.0;

		for (int i = 0; i < num; i++) {
			auto cherry = factory();
			cherry->speed = spd;
			cherry->direction = i * (dirRange / num);
			createCherry(cherry);
		}
	}
}
