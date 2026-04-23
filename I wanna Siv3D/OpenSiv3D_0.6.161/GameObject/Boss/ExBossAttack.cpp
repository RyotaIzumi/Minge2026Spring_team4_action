#include "ExBossCherry.h"
#include "../../Audio/AudioAsset.h"
#include "../../StageManager/BossStageManager.h"

namespace Iwanna {
	void ExBossCherry::attack() {
		switch (nowAttackType) {
		case ExBossAttackType::Wait:// --- 待機状態 --- //
			if (waitStopwatch.isRunning()) {
				if (waitStopwatch.s() >= waitTime) {
					waitStopwatch.reset();
					nowAttackType = ExBossAttackType::Slide;
				}
			}
			break;

		case ExBossAttackType::SparkExpro: // --- ✨爆発攻撃 --- //
			switch (attackStep) {
			case 0://上向きへ回転
				attackStep++;
				break;
			case 1://回転が終わったら、剣を光らせるエフェクトと攻撃の出す
				if (getIsRotateFinished()) {
					sordCherriesManager->sparkSordBlade();
					rotateDirection(0, 0.6, false);
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
			case 4://基本位置に移動
				if (getIsRotateFinished()) {
					movePosition(Vec2{ pos.x, baseY }, 1.0, false);
					attackStep++;
				}
				break;
			case 5:
				if (getIsMoveFinished()) {
					startWait();
				}
				break;
			}

			break;

		case ExBossAttackType::SwingOne: // --- 振り下ろし攻撃 --- //
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
					AudioAsset(Sound::SORD_STRONG).playOneShot();
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
					movePosition(Vec2{ pos.x, baseY }, 1.0, false);
					rotateDirection(getBaseAngleDiff(), 1.2, false);
					attackStep++;
				}
				break;
			case 4:
				if (getIsRotateFinished()) {
					startWait();
				}
				break;
			}
			break;

		case ExBossAttackType::Fall: // --- 落下攻撃 --- //
			switch (attackStep) {
			case 0:// 上昇
				movePosition(Vec2{ pos.x, -500 }, 1.0, true);
				attackStep++;
				break;
			case 1://落下
				if (getIsMoveFinished()) {
					pos.x = playerPos.x;
					rotateDirection(180, 0.001, false);
					movePosition(Vec2{ playerPos.x, 340 }, 0.4, true);
					sordCherriesManager->setSordCanPlayerKill(true);
					attackStep++;
				}
				break;
			case 2://突き刺さった状態からの待機
				if (getIsMoveFinished()) {
					AudioAsset(Sound::BLOCKBREAK).playOneShot();
					bossStageManager->getCameraShake().shake(0.7, 30.0);
					movePosition(pos, 1.2, true);
					rotateDirection(0, 0.1, true);//攻撃判定出現時間
					attackStep++;
				}
				break;
			case 3://戻る
				if (getIsRotateFinished()) sordCherriesManager->setSordCanPlayerKill(false);
				if (getIsMoveFinished()) {
					movePosition(Vec2{ pos.x, baseY }, 1.0, false);
					rotateDirection(getBaseAngleDiff(), 1.2, false);
					attackStep++;
				}
				break;
			case 4:
				if (getIsRotateFinished()) {
					startWait();
				}
				break;
			}
			break;

		case ExBossAttackType::Slide: // --- 横方向攻撃 --- //
			switch (attackStep) {
			case 0:// まず回転
				if (isPlayerInRightSide) {
					rotateDirection(90, 0.5, false);
				}
				else {
					rotateDirection(-90, 0.5, false);
				}
				attackStep++;
				break;
			case 1:// 左右の画面外へ移動
				if (getIsRotateFinished()) {
					if (isPlayerInRightSide) {
						movePosition(Vec2{ Global::stageWidth + 400, pos.y }, 1.2, true);
					}
					else {
						movePosition(Vec2{ -400, pos.y }, 1.2, true);
					}
					attackStep++;
				}
				break;
			case 2://画面上部を移動			
				if (getIsMoveFinished()) {
					double targetY = 150;
					pos.y = targetY;
					textureAngle += 180;

					if (isPlayerInRightSide) {		
						movePosition(Vec2{ Global::stageWidth + 400, targetY }, 1.4, true);
					}
					else {
						movePosition(Vec2{ - 400, targetY }, 1.4, true);
					}
					attackStep++;
				}
				break;
			case 3://画面下部を移動
				if (getIsMoveFinished()) {
					double targetY = 480;
					pos.y = targetY;
					textureAngle += 180;

					if (isPlayerInRightSide) {					
						movePosition(Vec2{ Global::stageWidth + 400, targetY }, 0.9, true);
					}
					else {
						movePosition(Vec2{ - 400, targetY }, 1.0, true);
					}

					AudioAsset(Sound::SORD_STRONG).playOneShot();
					sordCherriesManager->setSordCanPlayerKill(true);
					attackStep++;
				}
				break;
			case 4://画面外上部へ移動&待機
				if (getIsMoveFinished()) {
					pos.x = playerPos.x;
					pos.y = -300;
					sordCherriesManager->setSordCanPlayerKill(false);
					rotateDirection(getBaseAngleDiff(), 0.01, false);
					attackStep++;
				}
				break;
			case 5:
				if (getIsRotateFinished()) {
					movePosition(Vec2{ pos.x, baseY }, 1.2, false);
					attackStep++;
				}
				break;
			case 6:
				if (getIsRotateFinished()) {
					startWait();
				}
				break;
			}
			break;
		}

		
	}

	void ExBossCherry::startWait() {
		attackStep = 0;
		nowAttackType = ExBossAttackType::Wait;
		waitStopwatch.start();
		isNowAttacking = false;
	}
}
