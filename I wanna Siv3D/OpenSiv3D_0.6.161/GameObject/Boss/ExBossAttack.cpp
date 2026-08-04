#include "ExBossCherry.h"
#include "../../Audio/AudioAsset.h"
#include "../../StageManager/BossStageManager.h"

namespace Iwanna {
	void ExBossCherry::attack() {
		switch (nowAttackType) {
		case ExBossAttackType::Wait:// --- 待機状態 --- //
			if (isThirdFormRetreatPending) {
				isThirdFormRetreatPending = false;
				nowAttackType = ExBossAttackType::ThirdFormRetreat;
				attackStep = 0;
				waitStopwatch.reset();
				break;
			}

			if (waitStopwatch.isRunning()) {
				if (waitStopwatch.s() >= waitTime) {
					waitStopwatch.reset();
					decideAttack();
					//nowAttackType = ExBossAttackType::Slide;
				}
			}
			break;

		case ExBossAttackType::ThirdFormRetreat:
			switch (attackStep) {
			case 0:
				sordCherriesManager->setSordCanPlayerKill(false);
				movePosition(Vec2{ pos.x, -420 }, 1.2, true);
				rotateDirection(getBaseAngleDiff(), 1.2, false);
				thirdFormRetreatStopwatch.reset();
				attackStep++;
				break;
			case 1:
				if (getIsMoveFinished() && getIsRotateFinished()) {
					thirdFormRetreatStopwatch.restart();
					attackStep++;
				}
				break;
			case 2:
				if (thirdFormRetreatStopwatch.sF() >= thirdFormRetreatWaitTime) {
					bossStageManager->startExBossThirdPhaseDarkening();
					isThirdFormRetreatFinished = true;
					thirdFormRetreatStopwatch.reset();
					thirdFormRetreatStopwatch.restart();
					attackStep++;
				}
				break;
			case 3:
			{
				if (!isThirdFormSummonSelected && bossStageManager->isExBossThirdPhaseDarkened()) {
					thirdFormSummonType = Random(2);
					switch (thirdFormSummonType) {
					case 0:
						bossStageManager->summonExBossThirdPhaseLowBoss();
						break;
					case 1:
						bossStageManager->summonExBossThirdPhaseBossCherry();
						break;
					case 2:
						bossStageManager->summonExBossThirdPhaseTayama();
						break;
					}
					isThirdFormSummonSelected = true;
				}

				bool isSummonedBossFinished = false;
				switch (thirdFormSummonType) {
				case 0:
					isSummonedBossFinished = bossStageManager->isExBossThirdPhaseLowBossFinished();
					break;
				case 1:
					isSummonedBossFinished = bossStageManager->isExBossThirdPhaseBossCherryFinished();
					break;
				case 2:
					isSummonedBossFinished = bossStageManager->isExBossThirdPhaseTayamaFinished();
					break;
				}

				if (thirdFormRetreatStopwatch.sF() >= thirdFormDarkeningWaitTime
					&& isThirdFormSummonSelected
					&& isSummonedBossFinished) {
					thirdFormRetreatStopwatch.reset();
					bossStageManager->finishExBossThirdPhaseLowBoss();
					nowAttackType = ExBossAttackType::ThirdFormReturn;
					attackStep = 0;
				}
				break;
			}
			}

			break;

		case ExBossAttackType::ThirdFormReturn:
			switch (attackStep) {
			case 0:
			{
				const Vec2 cameraCenter = bossStageManager->getExBossLockedCameraCenter();
				pos = Vec2{ cameraCenter.x, cameraCenter.y - Global::windowHeight / 2.0 - 120.0 };
				movePosition(Vec2{ cameraCenter.x, 304.0 }, 1.4, false);
				rotateDirection(getBaseAngleDiff(), 1.0, false);
				attackStep++;
				break;
			}
			case 1:
				if (getIsMoveFinished() && getIsRotateFinished()) {
					baseY = 304.0;
					baseCenterPos = pos;
					isThirdFormReturning = false;
					startWait();
				}
				break;
			}

			break;

		case ExBossAttackType::SparkExpro: // --- ✨爆発攻撃 --- //
			switch (attackStep) {
			case 0://上向きへ回転
				movePosition(pos, 0.1, false);
				attackStep++;
				break;
			case 1://回転が終わったら、剣を光らせるエフェクトと攻撃の出す
				if (getIsMoveFinished()) {
					sordCherriesManager->sparkSordBlade();
					rotateDirection(0, 0.6, false);
					attackStep++;
				}
				break;
			case 2:
				if (getIsRotateFinished()) {
					//✨と予備攻撃範囲を出す
					Sound::playOneShot(Sound::SPARK);
					const std::function<std::shared_ptr<Cherry>()>& spark1 = [&]() {
						return std::make_shared<EffectCherrySpark>(sordCherriesManager->getSordEdgePos(), 0.3);
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
					Sound::playOneShot(Sound::EXPRO);
					bossStageManager->createSordExproCherry(attackStartPos, false, [this]() { return std::make_shared<ExproCherry>(pos, 1.0);});

					//弾幕作成
					if (bossForm >= BossForm::Third) {
						for(int i=0;i < 2;i++) barrageAttack(CherryColorType::Sky);
					}

					rotateDirection(0, 1.2, true);//攻撃終わりまで待ち
					attackStep++;
				}
				break;
			case 4://基本位置に移動
				if (getIsRotateFinished()) {
					movePosition(Vec2{ pos.x, baseY }, 1.0, false);
					rotateDirection(getBaseAngleDiff(), 1.0, false);
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
					Sound::playOneShot(Sound::SORD_STRONG);
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

					// 第二形態以降は振り上げに派生
					if (bossForm >= BossForm::Second && getRandomChance(0.8)) {
						nowAttackType = ExBossAttackType::SwingTwo;
						attackStep = 0;
						break;
					}

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

		case ExBossAttackType::SwingTwo: // --- 振り下ろし派生攻撃2 --- //
			switch (attackStep) {
			case 0:
				attackStep++;
				break;
			case 1://振り上げ開始
				if (getIsRotateFinished() && getIsMoveFinished()) {
					Sound::playOneShot(Sound::SORD_STRONG);
					if (isPlayerInRightSide) {
						movePosition(Vec2{pos.x + 50, pos.y - 30}, 0.5, false);
						rotateDirection(-220, 0.3, false);
					}
					else {
						movePosition(Vec2{ pos.x - 50, pos.y - 30}, 0.5, false);
						rotateDirection(220, 0.3, false);
					}
					sordCherriesManager->setSordCanPlayerKill(true);
					if (isPlayerInRightSide) bossStageManager->createSordFallSwingShockWaveCherry(Vec2{ pos.x, 528}, 0, [this]() { return std::make_shared<ExproCherry>(pos, 1.0); });
					else bossStageManager->createSordFallSwingShockWaveCherry(Vec2{ pos.x, 528 }, 180, [this]() { return std::make_shared<ExproCherry>(pos, 1.0); });
					attackStep++;
				}
				break;
			case 2://振り上げた後の待機時間
				if (getIsRotateFinished() && getIsMoveFinished()) {
					rotateDirection(0, 0.07, true);
					sordCherriesManager->setSordCanPlayerKill(false);
					attackStep++;
				}
				break;
			case 3://戻る
				if (getIsRotateFinished()) {

					// 第二形態以降は爆破に派生
					if (bossForm >= BossForm::Second && getRandomChance(0.4)) {
						nowAttackType = ExBossAttackType::SparkExpro;
						attackStep = 0;
						break;
					}

					// 第二形態以降はさらに振り上げに派生
					if (bossForm >= BossForm::Second && getRandomChance(0.5)) {
						nowAttackType = ExBossAttackType::SwingThree;
						attackStep = 0;
						break;
					}

					// 第二形態以降はワープに派生
					if (bossForm >= BossForm::Second && getRandomChance(0.6) && playerDistance > 100) {
						nowAttackType = ExBossAttackType::Warp;
						attackStep = 0;
						break;
					}

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

		case ExBossAttackType::SwingThree: // --- 振り下ろし派生攻撃3 --- //
			switch (attackStep) {
			case 0:
				attackStep++;
				break;
			case 1://振り上げ開始
				if (getIsRotateFinished() && getIsMoveFinished()) {

					//弾幕作成
					if (bossForm >= BossForm::Third) {
						barrageAttack(CherryColorType::Orange);
					}

					Sound::playOneShot(Sound::SORD_STRONG);
					if (isPlayerInRightSide) {
						movePosition(Vec2{ pos.x + 160, pos.y }, 0.5, false);
						rotateDirection(360, 1.0, false);
					}
					else {
						movePosition(Vec2{ pos.x - 160, pos.y }, 0.5, false);
						rotateDirection(-360, 1.0, false);
					}
					sordCherriesManager->setSordCanPlayerKill(true);
					attackStep++;
				}
				break;
			case 2://振り上げた後の待機時間
				if (getIsRotateFinished() && getIsMoveFinished()) {
					rotateDirection(0, 0.2, true);
					sordCherriesManager->setSordCanPlayerKill(false);
					attackStep++;
				}
				break;
			case 3://戻る
				if (getIsRotateFinished()) {

					// 第二形態以降は爆破に派生
					if (bossForm >= BossForm::Second && getRandomChance(0.5)) {
						nowAttackType = ExBossAttackType::SparkExpro;
						attackStep = 0;
						break;
					}

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
					Sound::playOneShot(Sound::BLOCKBREAK);

					//弾幕作成
					if (bossForm >= BossForm::Third) {
						barrageAttack(CherryColorType::Blue);
					}

					bossStageManager->getCameraShake().shake(0.7, 30.0);
					movePosition(pos, 1.0, true);
					rotateDirection(0, 0.1, true);//攻撃判定出現時間
					attackStep++;
				}
				break;
			case 3://戻る
				if (getIsRotateFinished()) sordCherriesManager->setSordCanPlayerKill(false);
				if (getIsMoveFinished()) {

					// 第二形態以降は振り上げに派生
					if (bossForm >= BossForm::Second && getRandomChance(0.7) && playerDistance > 160) {
						nowAttackType = ExBossAttackType::FallSwing;
						attackStep = 0;
						break;
					}

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

		case ExBossAttackType::FallSwing: // --- 落下攻撃後の振り上げ攻撃 --- //
			switch (attackStep) {
			case 0://プレイヤーの位置に応じて、右か左に振りかぶる
				if (isPlayerInRightSide) {
					rotateDirection(20, 0.6, false);
					movePosition(Vec2{ playerPos.x - 115, pos.y + 30 }, 0.7, false);
				}
				else {
					rotateDirection(-20, 0.6, false);
					movePosition(Vec2{ playerPos.x + 115, pos.y + 30 }, 0.7, false);
				}

				attackStep++;
				break;
			case 1://振り上げ開始
				if (getIsRotateFinished() && getIsMoveFinished()) {
					Sound::playOneShot(Sound::SORD_STRONG);
					if (isPlayerInRightSide) {
						rotateDirection(-220, 0.3, false);
					}
					else {
						rotateDirection(220, 0.3, false);
					}
					sordCherriesManager->setSordCanPlayerKill(true);
					if(isPlayerInRightSide) bossStageManager->createSordFallSwingShockWaveCherry(Vec2{ pos.x, pos.y + 150 }, 0, [this]() { return std::make_shared<ExproCherry>(pos, 1.0); });
					else bossStageManager->createSordFallSwingShockWaveCherry(Vec2{ pos.x, pos.y + 150 }, 180, [this]() { return std::make_shared<ExproCherry>(pos, 1.0); });
					attackStep++;
				}
				break;
			case 2://振り上げた後の待機時間
				if (getIsRotateFinished()) {
					rotateDirection(0, 0.3, true);
					sordCherriesManager->setSordCanPlayerKill(false);
					attackStep++;
				}
				break;
			case 3://戻る
				if (getIsRotateFinished()) {

					// 第二形態以降は爆破に派生
					if (bossForm >= BossForm::Second && getRandomChance(0.4)) {
						nowAttackType = ExBossAttackType::SparkExpro;
						attackStep = 0;
						break;
					}

					// 第二形態以降はワープに派生
					if (bossForm >= BossForm::Second && getRandomChance(0.6) && playerDistance > 200) {
						nowAttackType = ExBossAttackType::Warp;
						attackStep = 0;
						break;
					}

					// 第二形態以降は回転切りに派生
					if (bossForm >= BossForm::Second && getRandomChance(0.6) && playerDistance > 160) {
						nowAttackType = ExBossAttackType::SwingThree;
						attackStep = 0;
						break;
					}

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

					if (bossForm >= BossForm::Third) {
						continueGenerateColor = randomChoiceBarrageAttacks.choice();
						generateBarrageCherryTimer.restart();
					}

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

				if (bossForm >= BossForm::Third && generateBarrageCherryTimer.reachedZero() && 0 < pos.x && pos.x < Global::stageWidth) {
					slideBarrageAttack(continueGenerateColor);
					generateBarrageCherryTimer.restart();
				}

				if (getIsMoveFinished()) {
					double targetY = 480;
					pos.y = targetY;
					textureAngle += 180;

					generateBarrageCherryTimer.pause();

					if (isPlayerInRightSide) {					
						movePosition(Vec2{ Global::stageWidth + 400, targetY }, 0.9, true);
					}
					else {
						movePosition(Vec2{ - 400, targetY }, 1.0, true);
					}

					Sound::playOneShot(Sound::SORD_STRONG);
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
				if (getIsMoveFinished()) {
					startWait();
				}
				break;
			}
			break;

		case ExBossAttackType::Warp: // --- ワープ振り下ろし攻撃 --- //
			switch (attackStep) {
			case 0://プレイヤーの位置に応じて、右か左に振りかぶる
				if (isPlayerInRightSide) {
					rotateDirection(30, 0.6, false);
					movePosition(pos,0.3, false);
				}
				else {
					rotateDirection(-30, 0.6, false);
					movePosition(pos,0.3, false);
				}

				attackStep++;
				break;
			case 1://ワープ開始
				if (getIsMoveFinished()) {
					bossStageManager->createWarpCurtainCherry(pos, [this]() { return std::make_shared<WarpCurtainCherry>(2.0); });
					Sound::playOneShot(Sound::BOSS_WARP);
					attackStep++;
				}
				break;
			case 2://画面外上部へ移動する
				if (getIsRotateFinished()) {

					//弾幕作成
					if (bossForm >= BossForm::Third) {
						barrageAttack(CherryColorType::Green);
					}

					pos.y = -300;
					movePosition(pos, 0.4, false);
					attackStep++;
				}
				break;
			case 3://光再出現
				if (getIsMoveFinished()) {
					if (isPlayerInRightSide) bossStageManager->createWarpCurtainCherry(Vec2{ playerPos.x + 150, playerPos.y }, [this]() { return std::make_shared<WarpCurtainCherry>(2.0); });
					else bossStageManager->createWarpCurtainCherry(Vec2{ playerPos.x - 150, playerPos.y }, [this]() { return std::make_shared<WarpCurtainCherry>(2.0); });
					
					movePosition(pos, 0.27, false);
					attackStep++;
				}
				break;
			case 4://本体出現
				if (getIsMoveFinished()) {
					if (isPlayerInRightSide) pos.x = playerPos.x + 150;
					else pos.x = playerPos.x - 150;
					pos.y = playerPos.y - 10;

					rotateDirection(0, 0.5, false);
					attackStep++;
				}
				break;
			case 5://振り下ろし開始
				if (getIsRotateFinished()) {
					Sound::playOneShot(Sound::SORD_STRONG);
					if (isPlayerInRightSide) {
						rotateDirection(230, 0.25, false);
					}
					else {
						rotateDirection(-230, 0.25, false);
					}
					sordCherriesManager->setSordCanPlayerKill(true);
					attackStep++;
				}
			case 6://振り下ろした後の待機時間
				if (getIsRotateFinished()) {
					rotateDirection(0, 0.7, true);
					sordCherriesManager->setSordCanPlayerKill(false);
					attackStep++;
				}
				break;
			case 7://戻る
				if (getIsRotateFinished()) {
					// 第二形態以降は振り上げに派生
					if (bossForm >= BossForm::Second && getRandomChance(0.5) && playerDistance > 100) {
						nowAttackType = ExBossAttackType::SwingTwo;
						attackStep = 0;
						break;
					}

					movePosition(Vec2{ pos.x, baseY }, 1.0, false);
					rotateDirection(getBaseAngleDiff(), 1.2, false);
					attackStep++;
				}
				break;
			case 8:
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

	void ExBossCherry::barrageAttack(CherryColorType type) {
		// 攻撃を呼び出す
			double throwDir, throwSpd;
				switch (type) {
				case CherryColorType::Red:
					throwDir = 60 + Random(60);
					throwSpd = 9 + Random(3);
					bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, BossCherryType::Red, *bossStageManager); });
					break;
				case CherryColorType::Blue:
					bossStageManager->createBlueLineCherry(30, 80, [this]() { return std::make_shared<BossFallBlueCherry>(pos, 1.0, BossCherryType::Blue); });
					Sound::playOneShot(Sound::CHERRYFALL);
					break;
				case CherryColorType::Yellow:
					throwDir = 60 + Random(60);
					throwSpd = 9 + Random(3);
					bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, BossCherryType::Yellow, *bossStageManager); });
					break;
				case CherryColorType::Green:
					throwDir = 90;
					throwSpd = 1;
					bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, BossCherryType::Green, *bossStageManager); });
					break;
				case CherryColorType::Orange:
					throwDir = 90;
					throwSpd = 12;
					bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, BossCherryType::Orange, *bossStageManager); });
					break;
				case CherryColorType::Sky:
					throwDir = 60 + Random(60);
					throwSpd = 9 + Random(3);
					bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, BossCherryType::Sky, *bossStageManager); });
					break;
				}
	}

	void ExBossCherry::slideBarrageAttack(CherryColorType type) {
		// 攻撃を呼び出す
		double throwDir, throwSpd;
		switch (type) {
		case CherryColorType::Red:
			throwDir = 60 + Random(60);
			throwSpd = 3 + Random(3);
			bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, BossCherryType::Red, *bossStageManager); });
			break;
		case CherryColorType::Blue:
			bossStageManager->createBlueLineCherry(30, 80, [this]() { return std::make_shared<BossFallBlueCherry>(pos, 1.0, BossCherryType::Blue); });
			Sound::playOneShot(Sound::CHERRYFALL);
			break;
		case CherryColorType::Yellow:
			throwDir = 60 + Random(60);
			throwSpd = 3 + Random(3);
			bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, BossCherryType::Yellow, *bossStageManager); });
			break;
		case CherryColorType::Green:
			throwDir = 90;
			throwSpd = 1;
			bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, BossCherryType::Green, *bossStageManager); });
			break;
		case CherryColorType::Orange:
			throwDir = 90;
			throwSpd = 13;
			bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, BossCherryType::Orange, *bossStageManager); });
			break;
		case CherryColorType::Sky:
			throwDir = 60 + Random(60);
			throwSpd = 3 + Random(3);
			bossStageManager->createSubThrowCherry(throwDir, throwSpd, [this]() { return std::make_shared<BossSubThrowCherry>(pos, 2.0, BossCherryType::Sky, *bossStageManager); });
			break;
		}
	}
}
