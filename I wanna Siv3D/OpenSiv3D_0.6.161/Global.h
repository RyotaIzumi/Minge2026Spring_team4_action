#pragma once

namespace Global {
	// ======================
   // Enum 定義
   // ======================

	enum class Direction {
		LEFT,
		RIGHT
	};

	// ======================
	// 共有変数宣言
	// ======================

	//操作入力用変数
	inline Input inputLeft = KeyLeft;
	inline Input inputRight = KeyRight;
	inline Input inputJump = KeyShift;
	inline Input inputShoot = KeyZ;
	inline Input inputWarpMode = KeyX;
	inline Input inputStart = KeyShift;
	inline Input inputRestart = KeyR;
	inline Input inputEscape = KeyQ;

	inline Input inputDebugPause = Key1;
	inline Input inputDebugStart = Key2;
	inline Input inputDebugMuteki = KeyControl;

	// --- save関連 ---

	// ゲームが開始しているかどうか
	inline bool isExistSaveData = false;
	// セーブされたプレイヤーの開始座標
	inline Vec2 savedStartPlayerPos = Vec2(-100, -100);
	// セーブされた部屋名
	inline String savedRoomName = U"";
	// セーブされた武器モード。false: 銃, true: ワープ
	inline bool savedIsWarpMode = false;

	// --- room関連 ---
	inline int32 stageWidth = 800;
	inline int32 stageHeight = 608;
	inline bool isChangeRoom = false;
	inline String startRoomName = U"ExGotArea";
	inline String prevRoomName = U"";
	inline String nowRoomName = U"";
	inline bool isLoopStage = false;
	inline bool isSecretTriggerActivated = false;

	// --- 特殊罠関連 ---
	inline bool warningTrapPaused = false;
	//trap2 map専用
	inline bool trapActivatedInTrap2Map = false;
	inline bool trapActivatedId30InTrap2Map = false;
	inline bool trapCameraActivatedInTrap2Map = false;
	inline bool trap2MapBgmStop = false;

	// --- player関連 ---
	inline bool isPlayerFrozen = false;

	// --- boss関連 ---
	inline bool isBossAttackPowerUp = false;
	inline bool isBossExBarrageAttack = false;
	inline bool isBossDefeated = false;

	// --- カメラ関連 ---
	inline bool isCameraFollowMode = false;

	// --- BGM関連 --- ///
	inline bool doNotStopBgm = false;
	inline double soundVolume = 1.0;
	inline bool isFullscreen = false;

	// --- 入手アイテム関連 --- //
	inline bool prepareGetItem1 = false;
	inline bool getItem1 = false;
	inline bool getItem2 = false;

	inline int32 mainBgmNumber = 0;
	inline int32 mainTextureNumber = 0;
	inline int32 moraleValue1 = 50;
	inline int32 moraleValue2 = 50;
	inline int32 moraleValue3 = 50;
	inline int32 moraleValue4 = 50;

	// --- プレイ記録関連 --- //
	inline double elapsedPlayTime = 0.0;
	inline int32 deathCount = 0;
	inline int32 endingValue = 0;
	inline bool isRestartRoomReload = false;
	inline bool isLow1RestartDeathCheckActive = false;
	inline double low1RestartDeathCheckElapsed = 0.0;
	inline double low1RestartDeathCheckDuration = 0.05;

	// --- generateステージ関連 --- //
	inline Array<String> generateStageNames = { U"generate1", U"generate2", U"generate3", U"generate4", U"generate5" };
	inline Array<String> remainingGenerateStageNames;
	inline bool isGenerateStageFakeLoading = false;

	inline bool isGenerateStage(const String& roomName) {
		return generateStageNames.includes(roomName);
	}

	inline bool isExtraStage(const String& roomName) {
		return roomName == U"ExMiluArea"
			|| roomName == U"ExMochiArea"
			|| roomName == U"ExGotArea"
			|| roomName == U"ExRyutaArea"
			|| roomName == U"ExRenoArea";
	}

	inline bool isNoMoraleEndingRoute() {
		return moraleValue1 < 30
			&& moraleValue2 < 30
			&& moraleValue3 < 30
			&& moraleValue4 < 30;
	}

	inline bool isEndingKRoute() {
		return moraleValue1 >= 101
			&& moraleValue2 >= 101
			&& moraleValue3 >= 101
			&& moraleValue4 >= 101;
	}

	inline bool isEndingGRoute() {
		return !isEndingKRoute()
			&& moraleValue2 >= 90
			&& moraleValue3 >= 90
			&& moraleValue4 >= 90;
	}

	inline bool isEndingDRoute() {
		return !isEndingKRoute()
			&& !isNoMoraleEndingRoute()
			&& !isEndingGRoute()
			&& moraleValue1 >= 90;
	}

	inline bool isEndingBRoute() {
		return !isNoMoraleEndingRoute()
			&& moraleValue1 < 30
			&& moraleValue2 < 30;
	}

	inline bool isEndingCRoute() {
		return !isNoMoraleEndingRoute()
			&& !isEndingDRoute()
			&& !isEndingBRoute()
			&& moraleValue3 < 30
			&& moraleValue4 < 30;
	}

	inline bool isEndingHRoute() {
		return !isEndingGRoute()
			&& !isEndingDRoute()
			&& !isEndingBRoute()
			&& !isEndingCRoute()
			&& (30 <= moraleValue1 && moraleValue1 < 90)
			&& (30 <= moraleValue2 && moraleValue2 < 90)
			&& (30 <= moraleValue3)
			&& (30 <= moraleValue4);
	}

	inline bool canReachEndingFRoute() {
		return isEndingBRoute() || getItem1;
	}

	inline int32 getInitialEndingValue() {
		if (isNoMoraleEndingRoute()) {
			return 9;
		}
		if (isEndingDRoute()) {
			return 3;
		}
		return 4;
	}

	inline bool isItemEffectBlockedRoute() {
		return startRoomName == U"tutorialTrap" || isExtraStage(startRoomName);
	}

	inline bool canUseItem1Effect() {
		return getItem1 && !isItemEffectBlockedRoute();
	}

	inline bool canUseItem2Effect() {
		return getItem2 && !isItemEffectBlockedRoute();
	}

	inline bool canUseExBossItem2Effect() {
		return getItem2 && nowRoomName == U"ExBoss";
	}

	inline bool canShowDeathBloodEffect() {
		return moraleValue1 >= 40;
	}

	inline bool canUseMenuSettings() {
		return moraleValue1 >= 50;
	}

	inline String chooseGenerateStage() {
		if (remainingGenerateStageNames.isEmpty()) {
			remainingGenerateStageNames = generateStageNames;
		}

		const int32 index = Random(static_cast<int32>(remainingGenerateStageNames.size() - 1));
		const String roomName = remainingGenerateStageNames[index];
		remainingGenerateStageNames.remove_at(index);
		return roomName;
	}


	 // ======================
	// 定数
	// ======================

	// ゲームのフレームレート
	constexpr int32 FPS = 50;

	// ゲームウィンドウサイズ
	constexpr int32 windowWidth = 800;
	constexpr int32 windowHeight = 608;

	// 各チャプター開始ステップ
	constexpr int32 startStep_Chapter1 = 0;
	constexpr int32 startStep_Chapter2 = 840;
	constexpr int32 startStep_Chapter3 = 1320;

	inline void applyWindowMode() {
		const Size windowSize{ windowWidth, windowHeight };
		Scene::SetResizeMode(ResizeMode::Keep);
		if (isFullscreen) {
			Window::Resize(windowSize);
			Scene::Resize(windowSize);
			Window::SetFullscreen(true);
		}
		else {
			Window::SetFullscreen(false);
			Window::Resize(windowSize);
			Scene::Resize(windowSize);
		}
	}
}
