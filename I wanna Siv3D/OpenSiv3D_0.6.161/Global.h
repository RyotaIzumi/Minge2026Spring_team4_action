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

	inline Input inputDebugPause = Key1;
	inline Input inputDebugStart = Key2;
	inline Input inputDebugMuteki = KeyControl;

	// --- save関連 ---

	// ゲームが開始しているかどうか
	inline bool isExistSaveData = false;
	// セーブされたプレイヤーの開始座標
	inline Vec2 savedStartPlayerPos = Vec2(-100, -100);

	// --- room関連 ---
	inline int32 stageWidth = 800;
	inline int32 stageHeight = 608;

	// 特殊罠関連
	inline bool warningTrapPaused = false;

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
