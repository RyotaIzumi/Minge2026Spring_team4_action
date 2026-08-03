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
		Global::prevRoomName = U"";
		Global::nowRoomName = Global::startRoomName;
		Global::isChangeRoom = false;

		startGame();
	}

	void MainGame::startGame() {
		int32 chapter = 1;

		//ステージ名称系の初期化
		if (!Global::isExistSaveData && !Global::isChangeRoom) {
			Global::savedRoomName = Global::startRoomName;
			Global::nowRoomName = Global::startRoomName;
		}

		if(!Global::isChangeRoom) Global::nowRoomName = Global::savedRoomName;

		//ステージの名称から種類を決定
		if (Global::nowRoomName == U"boss" || Global::nowRoomName == U"bossLow" || Global::nowRoomName == U"ExBoss" || Global::nowRoomName == U"trapBoss") {
			stageType = StageType::Boss;
			pauseBgm();
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
				if (Global::nowRoomName == U"secret1" && nowSoundName != U"secret_stage")playBgm(U"secret_stage");
				if (Global::nowRoomName != U"secret1" && nowSoundName != mainBgmName)playBgm(mainBgmName);
			}
			else {
				if (Global::savedRoomName == U"secret1" && nowSoundName != U"secret_stage")playBgm(U"secret_stage");
				if (Global::savedRoomName != U"secret1" && nowSoundName != mainBgmName)playBgm(mainBgmName);
			}
		}

		switch (stageType) {
		case StageType::Normal:stageManager.setUpObjects(chapter); break;
		case StageType::Boss:bossStageManager.setUpObjects(chapter); break;
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
				if (playGameoverBgmOne && !Global::doNotStopBgm) {
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
			if (bossStageManager.getPlayer()->getIsDead() && !Global::doNotStopBgm) {
				if (playGameoverBgmOne) {
					playGameoverBgm();
					playGameoverBgmOne = false;
				}
				return;
			}
			//bossが出現したらBGM再生
			if (bossStageManager.bossBgmStart) {
				if(Global::nowRoomName == U"boss" || Global::nowRoomName == U"bossLow") playBgm(U"boss_normal");
				if(Global::nowRoomName == U"trapBoss") {
					if (bossStageManager.isTrapBossSecondPhaseBgm()) playBgm(U"boss_normal");
					else playBgm(U"boss_low", bossLowBgmVolume);
				}
				if(Global::nowRoomName == U"ExBoss") playBgm(U"ex_boss");
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
		audio.setVolume(volume);
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
