#include "ExBossCherry.h"
#include "../../Audio/AudioAsset.h"
#include "../../StageManager/BossStageManager.h"

namespace Iwanna {
	void ExBossCherry::attack() {
		switch (nowAttackType) {
		case ExBossAttackType::SparkExpro: // ✨爆発攻撃
			switch (attackStep) {
			case 0://上向きへ回転
				attackStep++;
				break;
			case 1://回転が終わったら、剣を光らせるエフェクトと攻撃の出す
				if (getIsRotateFinished()) {
					sordCherriesManager->sparkSordBlade();
					rotateDirection(0, 0.7, false);
					attackStep++;
				}
				break;
			case 2:
				if (getIsRotateFinished()) {
					//✨と予備攻撃範囲を出す
					AudioAsset(Sound::SPARK).playOneShot();
					const std::function<std::shared_ptr<Cherry>()>& spark1 = [&]() {
						return std::make_shared<EffectCherrySpark>(Vec2{ sordCherriesManager->pos.x, sordCherriesManager->pos.y - 160 }, 0.3);
					};
					const std::function<std::shared_ptr<Cherry>()>& spark2 = [&]() {
						return std::make_shared<EffectCherrySpark>(Vec2{ sordCherriesManager->pos.x, sordCherriesManager->pos.y - 160 }, 0.02);
					};
					attackStartPos = { -100 + Random(100), 512 };
					bossStageManager->createCherry(spark1());
					bossStageManager->createSordExproCherry(attackStartPos,true,spark2);
					rotateDirection(0, 0.8, true);
					attackStep++;
				}
				break;
			case 3:
				if (getIsRotateFinished()) {
					//攻撃を出す
					AudioAsset(Sound::EXPRO).playOneShot();
					bossStageManager->createSordExproCherry(attackStartPos, false, [this]() { return std::make_shared<ExproCherry>(pos, 1.0);});

					rotateDirection(0, 1.2, true);//攻撃終わりまで待ち
					attackStep++;
				}
				break;
			case 4:
				if (getIsRotateFinished()) {
					movePosition(Vec2{ pos.x, baseY }, 1.1, false);
					attackStep++;
				}
				break;
			case 5:
				if (getIsMoveFinished()) {
					attackStep = 0;
					isNowAttacking = false;
					nowAttackType = ExBossAttackType::None;
				}
				break;
			}

			break;

		case ExBossAttackType::SwingOne: // 振り下ろし攻撃
			switch (attackStep) {
			case 0://プレイヤーの位置に応じて、右か左に振りかぶる
				if (isPlayerInRightSide) {
					rotateDirection(-20, 0.8, false);
					movePosition(Vec2{ playerPos.x - 135, playerPos.y }, 1.0, false);
				}
				else {
					rotateDirection(20, 0.8, false);
					movePosition(Vec2{ playerPos.x + 135, playerPos.y }, 1.0, false);
				}

				attackStep++;
				break;
			case 1://振り下ろし開始
				if (getIsRotateFinished() && getIsMoveFinished()) {
					if (isPlayerInRightSide) {
						rotateDirection(220, 0.2, false);
					}
					else {
						rotateDirection(-220, 0.2, false);
					}
					sordCherriesManager->setSordCanPlayerKill(true);
					attackStep++;
				}
				break;
			case 2://振り下ろした後の待機時間
				if (getIsRotateFinished()) {
					rotateDirection(0, 0.7, true);
					sordCherriesManager->setSordCanPlayerKill(false);
					attackStep++;
				}
				break;
			case 3://戻る
				if (getIsRotateFinished()) {
					rotateDirection(getBaseAngleDiff(), 1.2, false);
					attackStep++;
				}
				break;
			case 4:
				if (getIsRotateFinished()) {
					attackStep = 0;
					nowAttackType = ExBossAttackType::None;
				}
				break;
			}
		}


	}
}
