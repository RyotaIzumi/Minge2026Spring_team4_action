#include "MainGame.h"

namespace Iwanna {
	MainGame::MainGame() {
	}

	void MainGame::startGame() {
		int32 chapter = 1;

		//ステージ名称系の初期化
		if (!Global::isExistSaveData && !Global::isChangeRoom) {
			Global::savedRoomName = Global::startRoomName;
			Global::nowRoomName = Global::startRoomName;
		}

		//ステージの名称から種類を決定
		if (Global::nowRoomName == U"boss" || Global::nowRoomName == U"ExBoss") {
			stageType = StageType::Boss;
			if (Global::nowRoomName == U"boss" || Global::nowRoomName == U"ExBoss")pauseBgm();
		}
		else {
			stageType = StageType::Normal;
		}

		if (Global::isChangeRoom) {
			if (Global::nowRoomName == U"secret1")playBgm(U"secret_stage");
		}

		switch (stageType) {
		case StageType::Normal:stageManager.setUpObjects(chapter); break;
		case StageType::Boss:bossStageManager.setUpObjects(chapter); break;
		}

		playGameoverBgmOne = true;
		gameoverAudio.stop();

		if (stageType == StageType::Boss)return;

		//BGM再生関連
		if (!audio.isPlaying()) {
			if (audio.isPaused())audio.play();
			else playBgm(U"main_normal");
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

			if (Global::bgmStop) {
				pauseBgm();
			}

			//playerが死亡していたらBGM一時停止
			if (stageManager.getPlayer()->getIsDead()) {
				if (playGameoverBgmOne) {
					playGameoverBgm();
					playGameoverBgmOne = false;
				}
				return;
			}
			stageManager.getPlayer()->setStopOrPlayAnimation(!Global::warningTrapPaused);
			break;

		case StageType::Boss:
			bossStageManager.update();
			//playerが死亡していたらBGM一時停止
			if (bossStageManager.getPlayer()->getIsDead()) {
				if (playGameoverBgmOne) {
					playGameoverBgm();
					playGameoverBgmOne = false;
				}
				return;
			}
			//bossが出現したらBGM再生
			if (bossStageManager.bossBgmStart) {
				if(Global::nowRoomName == U"boss") playBgm(U"boss_normal");
				if(Global::nowRoomName == U"ExBoss") playBgm(U"ex_boss");
				bossStageManager.bossBgmStart = false;
			}
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

	void MainGame::playBgm(String bgm) {
		stopBgm();
		
		audio = AudioAsset{bgm};
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
		if(!gameoverAudio.isPlaying()) gameoverAudio.play();
	}

	void MainGame::stopBgm() {
		audio.stop();
	}

	void MainGame::pauseBgm() {
		audio.pause();
	}
}
