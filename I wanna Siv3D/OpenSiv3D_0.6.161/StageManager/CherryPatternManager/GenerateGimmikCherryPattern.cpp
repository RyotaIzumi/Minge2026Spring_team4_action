#include "../StageManager.h"

namespace Iwanna {
	// 汎用性の高い生成パターン関数を集めたファイル

	/**
		 * @brief りんご拡散弾生成
		 * @param num 生成数
		 * @param spd 速度
		 * @param cherry 生成するcherryオブジェクト
		 */
	void StageManager::createCherrySpread(int32 num, double spd, const std::function<std::shared_ptr<Cherry>()>& factory) {
		const double dirRange = 360.0;

		for (int i = 0; i < num; i++) {
			auto cherry = factory();
			cherry->speed = spd;
			cherry->direction = i * (dirRange / num);
			createCherry(cherry);
		}
	}

	/**
		 * @brief ボスの投げるサブりんご生成
		 * @param dir 角度
		 * @param spd 速度
		 * @param cherry 生成するcherryオブジェクト
		 */
	void StageManager::createSubThrowCherry(double dir, double spd, const std::function<std::shared_ptr<Cherry>()>& factory) {

		auto cherry = factory();
		cherry->speed = spd;
		cherry->direction = dir;
		createCherry(cherry);
	}

	/**
		 * @brief 青攻撃用のライン型
		 * @param cherry 生成するcherryオブジェクト
		 */
	void StageManager::createBlueLineCherry(const std::function<std::shared_ptr<Cherry>()>& factory) {
		const int32 num = 15;
		const double interval = 64;
		double startX = 16;

		for (int i = 0; i < num; i++) {
			auto cherry = factory();
			cherry->pos.x = interval * i + startX;
			cherry->pos.y = -100;
			createCherry(cherry);
		}
	}

	/**
		 * @brief 黄色攻撃用の星型
		 * @param cherry 生成するcherryオブジェクト
		 */
	void StageManager::createYellowStarCherry(int32 Nkakkei, int32 nextNumber, Vec2 center, int32 lineNum, const std::function<std::shared_ptr<Cherry>()>& factory) {
		int32 r = 300;
		Array<std::shared_ptr<Cherry>> starCherries;
		double angleStart = 270;

		//外周のみ生成
		for (int j = 0; j < 2; j++) {
			for (int i = 0; i < Nkakkei; i++) {
				auto cherry = factory();
				cherry->pos.x = r * cos(Math::ToRadians(i * (360.0 / Nkakkei) + angleStart)) + center.x;
				cherry->pos.y = r * sin(Math::ToRadians(i * (360.0 / Nkakkei) + angleStart)) + center.y;
				cherry->direction = calculateDirection(center, cherry->pos);
				starCherries << cherry;
				createCherry(cherry);
			}
		}

		//内側の線も生成
		for (int i = 0; i < Nkakkei; i++) {
			for (int j = 0; j < lineNum; j++) {
				auto cherry = factory();
				cherry->pos.x = starCherries[i]->pos.x + (starCherries[i + nextNumber]->pos.x - starCherries[i]->pos.x) * j / lineNum;
				cherry->pos.y = starCherries[i]->pos.y + (starCherries[i + nextNumber]->pos.y - starCherries[i]->pos.y) * j / lineNum;
				cherry->direction = calculateDirection(center, cherry->pos);
				createCherry(cherry);
			}
		}
	}

	/**
		 * @brief 緑攻撃用のウェーブ型
		 * @param startPos 生成位置
		 * @param interval 上昇間隔
		 * @param cherry 生成するcherryオブジェクト
		 */
	void StageManager::createGreenWaveCherry(Vec2 startPos, double interval, double high, const std::function<std::shared_ptr<BarrageGimmikGreenCherry>()>& factory) {
		const double startX = -1 * Random(interval);
		const double interX = 16;
		const int32 num = 70;
		//右側
		for (int i = 0; i < num; i++) {
			auto cherry = factory();
			cherry->pos.x = startPos.x + interX * i;
			cherry->pos.y = Global::stageHeight + 30;
			cherry->highSpeed = high;
			cherry->setActiveTimer(interval * i);

			createCherry(cherry);
		}
	}

	/**
		 * @brief オレンジ攻撃用のライン型
		 * @param targetPos 目標位置
		 * @param interval 上昇間隔
		 * @param cherry 生成するcherryオブジェクト
		 */
	void StageManager::createOrangeStopCherry(bool isAddUpDown, const std::function<std::shared_ptr<BossOrangeStopCherry>()>& factory) {
		const double inter = 20;
		const int32 num = 50;
		const Vec2 targetPos = gameObjects.player->pos;

		//右側
		for (int i = 0; i < num; i++) {
			auto cherry = factory();
			cherry->pos.x = Global::stageWidth + inter;
			cherry->pos.y = -inter + inter * i;
			cherry->setStartPos(cherry->pos);
			cherry->setTargetPos(Vec2{ targetPos.x + inter, cherry->pos.y });

			createCherry(cherry);
		}

		if (isAddUpDown) {
			//上側
			for (int i = 0; i < num; i++) {
				auto cherry = factory();
				cherry->pos.x = -inter + inter * i;
				cherry->pos.y = -inter;
				cherry->setStartPos(cherry->pos);
				cherry->setTargetPos(Vec2{ cherry->pos.x, targetPos.y - (inter + 10) });
				createCherry(cherry);
			}

			//下側
			for (int i = 0; i < num; i++) {
				auto cherry = factory();
				cherry->pos.x = -inter + inter * i;
				cherry->pos.y = Global::stageHeight + inter;
				cherry->setStartPos(cherry->pos);
				cherry->setTargetPos(Vec2{ cherry->pos.x, targetPos.y + (inter + 10) });
				createCherry(cherry);
			}
		}

		cameraShake.shake(0.4, 20.0);
	}

	/**
		 * @brief 水色攻撃用の星型
		 * @param cherry 生成するcherryオブジェクト
		 * @param lineNum １線内の弾幕数
		 * @param isAddLine 左右に弾幕を追加するかどうか
		 */
	void StageManager::createSkyTargetCherry(int32 lineNum, const std::function<std::shared_ptr<BarrageCherry>()>& factory) {
		Vec2 targetPos = gameObjects.player->pos;
		double baseSpd = 1;
		double interSpd = 1.7;
		double interAngle = 20;

		for (int i = 0; i < lineNum; i++) {
			auto cherry = factory();
			cherry->direction = calculateDirection(cherry->pos, targetPos);
			cherry->speed = baseSpd + i * interSpd;
			createCherry(cherry);
		}
	}

	/**
		 * @brief 灰色攻撃用の格子型
		 * @param cherry 生成するcherryオブジェクト
		 */
	void StageManager::createGrayLatticeCherry(double interval, const std::function<std::shared_ptr<BossGrayLatticeCherry>()>& factory) {
		Vec2 startBasePos{ -100,-400 };
		Vec2 centerPos{ 400,150 };
		double cherryInterval = 24;
		int32 lineNumX = 15;
		int32 lineNumY = 15;
		int32 cherryNumInLineX = 70;
		int32 cherryNumInLineY = 70;
		double randomAngle = Random(360);

		//横方向
		for (int i = 0; i < lineNumX; i++) {
			for (int j = 0; j < cherryNumInLineX; j++) {
				auto cherry = factory();
				cherry->pos.x = startBasePos.x + j * cherryInterval;
				cherry->pos.y = startBasePos.y + interval * i;
				cherry->setCenterPos(centerPos);
				cherry->setDistanceAndAngle(calculateDistance(centerPos, cherry->pos), calculateDirection(centerPos, cherry->pos) + randomAngle);
				cherry->setAttackPattern(0);
				createCherry(cherry);
			}
		}

		//縦方向
		for (int i = 0; i < lineNumY; i++) {
			for (int j = 0; j < cherryNumInLineY; j++) {
				auto cherry = factory();
				cherry->pos.x = startBasePos.x + interval * i;
				cherry->pos.y = startBasePos.y + j * cherryInterval;
				cherry->setCenterPos(centerPos);
				cherry->setDistanceAndAngle(calculateDistance(centerPos, cherry->pos), calculateDirection(centerPos, cherry->pos) + randomAngle);
				cherry->setAttackPattern(0);
				createCherry(cherry);
			}
		}

		int32 cherryNumCircle = 60;
		double circleAngleInterval = 360 / cherryNumCircle;
		//最初の予告円形
		for (int32 i = 0; i <= cherryNumCircle; i++) {
			auto cherry = factory();
			cherry->setCenterPos(centerPos);
			cherry->setDistanceAndAngle(0, i * circleAngleInterval);
			cherry->setAttackPattern(1);
			createCherry(cherry);
		}

		//2つ目の予告円形
		for (int32 i = 0; i <= cherryNumCircle; i++) {
			auto cherry = factory();
			cherry->setCenterPos(centerPos);
			cherry->setDistanceAndAngle(0, i * circleAngleInterval);
			cherry->setAttackPattern(2);
			createCherry(cherry);
		}
	}
}
