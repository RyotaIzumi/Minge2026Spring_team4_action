#include "MainGame.h"

namespace Iwanna {
	MainGame::MainGame() {
	}

	void MainGame::startGame() {
		int32 chapter = 1;
		stageManager.setUpObjects(chapter);

		//BGM再生関連
		if (!audio.isPlaying()) {
			if (audio.isPaused())audio.play();
			else playBgm(chapter);
		}
	}

	void MainGame::updateGame() {
		
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

		if (stageManager.getStageName() == U"boss") {
			stopBgm();
		}

		stageManager.getPlayer()->setStopOrPlayAnimation(!Global::warningTrapPaused);
	}

	void MainGame::debugGame() {
		stageManager.debug();
		if (Global::inputDebugPause.down())pauseBgm();
		if (Global::inputDebugStart.down())audio.play();
	}

	void MainGame::drawGame() {
		stageManager.draw();
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
