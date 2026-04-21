#include "ExBossCherry.h"
#include "../../Audio/AudioAsset.h"
#include "../../StageManager/BossStageManager.h"

namespace Iwanna {
	void ExBossCherry::attack() {
		switch (nowAttackType) {
		case ExBossAttackType::SparkExpro:

			switch (attackStep) {
			case 0:
				sordCherriesManager->sparkSordBlade();
				rotateDirection(0, 1.0, true);
				attackStep++;
				break;
			case 1:
				if (getIsRotateFinished()) {
					//✨と予備攻撃範囲を出す
					rotateDirection(0, 1.0, true);
					startStep++;
				}
				break;
			case 2:
				if (getIsRotateFinished()) {
					//攻撃を出す
					rotateDirection(0, 1.0, true);
					startStep++;
				}
				break;
			}

			break;
		}
	}
}
