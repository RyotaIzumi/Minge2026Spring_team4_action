#include "MainGame.h"

namespace Iwanna {
	MainGame::MainGame() {
	}

	void MainGame::startNewGame() {
		// タイトル画面からの開始時は、以前のプレイ中に保持された
		// セーブ先や遷移先ではなく、設定された開始ルームを必ず使用する
		Global::isExistSaveData = false;
		Global::savedStartPlayerPos = Vec2{ -100, -100 };
		Global::savedRoomName = Global::startRoomName;
		Global::savedIsWarpMode = false;
		Global::prevRoomName = U"";
		Global::nowRoomName = Global::startRoomName;
		Global::isChangeRoom = false;
		Global::endingDGenerateClearCount = 0;

		startGame();
	}

	void MainGame::resetGameToStartMenu() {
		audio.stop();
		gameoverAudio.stop();
		nowSoundName = U"";
		playGameoverBgmOne = true;

		Global::isExistSaveData = false;
		Global::savedStartPlayerPos = Vec2{ -100, -100 };
		Global::savedRoomName = Global::startRoomName;
		Global::savedIsWarpMode = false;
		Global::prevRoomName = U"";
		Global::nowRoomName = Global::startRoomName;
		Global::isChangeRoom = false;
		Global::isLoopStage = false;

		Global::trap2MapBgmStop = false;
		Global::trapActivatedInTrap2Map = false;
		Global::trapActivatedId30InTrap2Map = false;
		Global::trapCameraActivatedInTrap2Map = false;
		Global::warningTrapPaused = false;
		Global::isPlayerFrozen = false;
		Global::isSecretTriggerActivated = false;
		Global::isBossAttackPowerUp = false;
		Global::isBossExBarrageAttack = false;
		Global::isBossDefeated = false;
		Global::isCameraFollowMode = false;
		Global::doNotStopBgm = false;
		Global::prepareGetItem1 = false;

		Global::elapsedPlayTime = 0.0;
		Global::deathCount = 0;
		Global::isRestartRoomReload = false;
		Global::isLow1RestartDeathCheckActive = false;
		Global::low1RestartDeathCheckElapsed = 0.0;

		Global::remainingGenerateStageNames.clear();
		Global::isGenerateStageFakeLoading = false;
		Global::endingDGenerateClearCount = 0;

		Global::endingValue = Global::getInitialEndingValue();

		if (Global::moraleValue2 > 90) Window::SetTitle(U"TestPlayGame (Debug Build)");
		else Window::SetTitle(U"TestPlayGame");
	}

	void MainGame::startGame() {
		int32 chapter = 1;
		const bool startedByRoomChange = Global::isChangeRoom;

		//ステージ名称系の初期化
		if (!Global::isExistSaveData && !Global::isChangeRoom) {
			Global::savedRoomName = Global::startRoomName;
			Global::nowRoomName = Global::startRoomName;
			Global::savedIsWarpMode = false;
		}

		if(!Global::isChangeRoom) Global::nowRoomName = Global::savedRoomName;

		//ステージの名称から種類を決定
		if (Global::nowRoomName == U"boss" || Global::nowRoomName == U"bossLow" || Global::nowRoomName == U"ExBoss" || Global::nowRoomName == U"trapBoss") {
			stageType = StageType::Boss;
			const bool keepExBossBgm = (Global::nowRoomName == U"ExBoss"
				&& Global::isExistSaveData
				&& !Global::isChangeRoom
				&& nowSoundName == U"ex_boss"
				&& audio.isPlaying());
			if (!keepExBossBgm) {
				pauseBgm();
			}
		}
		else {
			stageType = StageType::Normal;
		}

		String mainBgmName;
		switch (Global::mainBgmNumber) {
		case 0:mainBgmName = U"main_low"; break;
		case 1:mainBgmName = U"main_normal"; break;
		case 2:mainBgmName = U"main_high"; break;
		}

		if (stageType != StageType::Boss) {
			//BGM再生関連
			if (!audio.isPlaying()) {
				if (audio.isPaused())audio.play();
				else playBgm(mainBgmName);
			}

			if (Global::isChangeRoom) {
				const bool useSecretStageBgm = (Global::nowRoomName == U"secret1" || Global::isExtraStage(Global::nowRoomName));
				if (useSecretStageBgm && nowSoundName != U"secret_stage")playBgm(U"secret_stage");
				if (!useSecretStageBgm && nowSoundName != mainBgmName)playBgm(mainBgmName);
			}
			else {
				const bool useSecretStageBgm = (Global::savedRoomName == U"secret1" || Global::isExtraStage(Global::savedRoomName));
				if (useSecretStageBgm && nowSoundName != U"secret_stage")playBgm(U"secret_stage");
				if (!useSecretStageBgm && nowSoundName != mainBgmName)playBgm(mainBgmName);
			}
		}

		switch (stageType) {
		case StageType::Normal:stageManager.setUpObjects(chapter); break;
		case StageType::Boss:bossStageManager.setUpObjects(chapter); break;
		}

		Global::isLow1RestartDeathCheckActive = Global::isRestartRoomReload
			&& (Global::nowRoomName == U"low1" || Global::getItem1);
		Global::low1RestartDeathCheckElapsed = 0.0;
		Global::isRestartRoomReload = false;

		if (startedByRoomChange && Global::isGenerateStage(Global::nowRoomName)) {
			Global::isGenerateStageFakeLoading = true;
		}

		playGameoverBgmOne = true;
		gameoverAudio.stop();

		

		
	}

	void MainGame::updateGame() {

		switch (stageType) {
		case StageType::Normal:
			if (!Global::warningTrapPaused) {
				stageManager.update();
			}
			else {
				stageManager.getWarningWindowTrap()->trapUpdate();
			}

			if (Global::trap2MapBgmStop) {
				pauseBgm();
			}

			//playerが死亡していたらBGM一時停止
			if (stageManager.getPlayer()->getIsDead()) {
				if (Global::isLow1RestartDeathCheckActive
					&& Global::canReachEndingFRoute()
					&& Global::low1RestartDeathCheckElapsed <= Global::low1RestartDeathCheckDuration) {
					Global::endingValue = 5;
					Global::isLow1RestartDeathCheckActive = false;
				}
				if (playGameoverBgmOne && !Global::doNotStopBgm) {
					playGameoverBgm();
					playGameoverBgmOne = false;
				}
				return;
			}
			if (Global::nowRoomName != U"clear") {
				Global::elapsedPlayTime += Scene::DeltaTime();
			}
			if (Global::isLow1RestartDeathCheckActive) {
				Global::low1RestartDeathCheckElapsed += Scene::DeltaTime();
				if (Global::low1RestartDeathCheckElapsed > Global::low1RestartDeathCheckDuration) {
					Global::isLow1RestartDeathCheckActive = false;
				}
			}
			stageManager.getPlayer()->setStopOrPlayAnimation(!Global::warningTrapPaused);
			break;

		case StageType::Boss:
			bossStageManager.update();
			//playerが死亡していたらBGM一時停止
			if (bossStageManager.getPlayer()->getIsDead()) {
				if (Global::isLow1RestartDeathCheckActive
					&& Global::canReachEndingFRoute()
					&& Global::low1RestartDeathCheckElapsed <= Global::low1RestartDeathCheckDuration) {
					Global::endingValue = 5;
					Global::isLow1RestartDeathCheckActive = false;
				}
				if (playGameoverBgmOne && !Global::doNotStopBgm) {
					playGameoverBgm();
					playGameoverBgmOne = false;
				}
				return;
			}
			if (Global::isLow1RestartDeathCheckActive) {
				Global::low1RestartDeathCheckElapsed += Scene::DeltaTime();
				if (Global::low1RestartDeathCheckElapsed > Global::low1RestartDeathCheckDuration) {
					Global::isLow1RestartDeathCheckActive = false;
				}
			}
			Global::elapsedPlayTime += Scene::DeltaTime();
			//bossが出現したらBGM再生
			if (bossStageManager.bossBgmStart) {
				if(Global::nowRoomName == U"boss" || Global::nowRoomName == U"bossLow") {
					if (Global::mainBgmNumber == 0) playBgm(U"boss_low", bossLowBgmVolume);
					else playBgm(U"boss_normal");
				}
				if(Global::nowRoomName == U"trapBoss") {
					if (bossStageManager.isTrapBossSecondPhaseBgm()) playBgm(U"boss_normal");
					else playBgm(U"boss_low", bossLowBgmVolume);
				}
				if (Global::nowRoomName == U"ExBoss") {
					if (nowSoundName != U"ex_boss" || (!audio.isPlaying() && !audio.isPaused())) {
						playBgm(U"ex_boss");
					}
					else if (audio.isPaused()) {
						audio.play();
					}
				}
				bossStageManager.bossBgmStart = false;
			}

			if (bossStageManager.shouldStopBossBgm())stopBgm();
			break;
		}
	}

	void MainGame::debugGame() {
		switch (stageType) {
		case StageType::Normal:
			stageManager.debug();
			break;
		case StageType::Boss:
			//bossStageManager.debug();
			break;
		}

		if (Global::inputDebugPause.down())pauseBgm();
		if (Global::inputDebugStart.down())audio.play();
	}

	void MainGame::drawGame() {
		switch (stageType) {
		case StageType::Normal:stageManager.draw(); break;
		case StageType::Boss:bossStageManager.draw(); break;
		}
	}

	void MainGame::playBgm(String bgm, double volume) {
		stopBgm();
		
		audio = AudioAsset{bgm};
		nowSoundName = bgm;
		currentBgmBaseVolume = volume;
		audio.setVolume(currentBgmBaseVolume * Global::soundVolume);
		/*
		SecondsF startTime = 0.0s;
		int32 startStep = 0;

		switch (chapter) {
		case 1:startStep = Global::startStep_Chapter1; break;
		case 2:startStep = Global::startStep_Chapter2; break;
		case 3:startStep = Global::startStep_Chapter3; break;
		}

		startTime = SecondsF(static_cast<double>(startStep) / static_cast<double>(Global::FPS));

		audio.seekTime(startTime);
		*/
		audio.play();

		//assert("うへぇ");
	}

	void MainGame::playGameoverBgm() {
		pauseBgm();
		gameoverAudio = AudioAsset{ U"gameover_normal" };
		gameoverAudio.setVolume(Global::soundVolume);
		if(!gameoverAudio.isPlaying()) gameoverAudio.play();
	}

	void MainGame::applySoundVolume() {
		audio.setVolume(currentBgmBaseVolume * Global::soundVolume);
		gameoverAudio.setVolume(Global::soundVolume);
	}

	void MainGame::stopBgm() {
		audio.stop();
	}

	void MainGame::pauseBgm() {
		audio.pause();
	}
}
