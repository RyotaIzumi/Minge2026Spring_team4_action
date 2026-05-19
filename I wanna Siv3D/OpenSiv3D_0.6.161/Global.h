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

	// --- room関連 ---
	inline int32 stageWidth = 800;
	inline int32 stageHeight = 608;
	inline bool isChangeRoom = false;
	inline String startRoomName = U"ExBoss";
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

	// --- 入手アイテム関連 --- //
	inline bool prepareGetItem1 = false;
	inline bool getItem1 = true;

	inline int32 mainBgmNumber = 0;
	inline int32 mainTextureNumber = 0;


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
}
