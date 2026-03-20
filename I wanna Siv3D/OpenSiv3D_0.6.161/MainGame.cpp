#include "MainGame.h"

namespace Iwanna {
	MainGame::MainGame() {
	}

	void MainGame::startGame() {
		int32 chapter = 1;

		//ステージ名称系の初期化
		if (!Global::isExistSaveData) {
			Global::savedRoomName = Global::startRoomName;
			Global::nowRoomName = Global::startRoomName;
		}

		//ステージの名称から種類を決定
		if (Global::nowRoomName == U"boss") {
			stageType = StageType::Boss;
		}
		else {
			stageType = StageType::Normal;
		}

		switch (stageType) {
		case StageType::Normal:stageManager.setUpObjects(chapter); break;
		case StageType::Boss:bossStageManager.setUpObjects(chapter); break;
		}

		//BGM再生関連
		if (!audio.isPlaying()) {
			if (audio.isPaused())audio.play();
			else playBgm(chapter);
		}
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

			//playerが死亡していたらBGM一時停止
			if (stageManager.getPlayer()->getIsDead() || Global::bgmStop) {
				pauseBgm();
				return;
			}
			stageManager.getPlayer()->setStopOrPlayAnimation(!Global::warningTrapPaused);
			break;

		case StageType::Boss:
			bossStageManager.update();
			//playerが死亡していたらBGM一時停止
			if (bossStageManager.getPlayer()->getIsDead() || Global::bgmStop) {
				pauseBgm();
				return;
			}
			break;
		}

		if(Global::nowRoomName == U"boss")pauseBgm();
	}

	void MainGame::debugGame() {
		switch (stageType) {
		case StageType::Normal:
			stageManager.debug();
		case StageType::Boss:
			bossStageManager.debug();
		}

		if (Global::inputDebugPause.down())pauseBgm();
		if (Global::inputDebugStart.down())audio.play();
	}

	void MainGame::drawGame() {
		switch (stageType) {
		case StageType::Normal:
			stageManager.draw();
		case StageType::Boss:
			bossStageManager.draw();
		}
	}

	void MainGame::playBgm(int32 chapter) {
		stopBgm();
		
		audio = AudioAsset{ U"main_normal"};
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
	}

	void MainGame::stopBgm() {
		audio.stop();
	}

	void MainGame::pauseBgm() {
		audio.pause();
	}
}
