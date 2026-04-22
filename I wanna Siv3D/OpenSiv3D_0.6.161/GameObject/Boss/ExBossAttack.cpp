#include "ExBossCherry.h"
#include "../../Audio/AudioAsset.h"
#include "../../StageManager/BossStageManager.h"

namespace Iwanna {
	void ExBossCherry::attack() {
		switch (nowAttackType) {
		case ExBossAttackType::SparkExpro:
			switch (attackStep) {
			case 0://上向きへ回転
				rotateDirection(180, 0.8, true);
				attackStep++;
				break;
			case 1://回転が終わったら、剣を光らせるエフェクトと攻撃の出す
				if (getIsRotateFinished()) {
					sordCherriesManager->sparkSordBlade();
					rotateDirection(0, 0.7, true);
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

					rotateDirection(0, 1.0, true);
					attackStep++;
				}
				break;
			}

			break;
		}
	}
}
