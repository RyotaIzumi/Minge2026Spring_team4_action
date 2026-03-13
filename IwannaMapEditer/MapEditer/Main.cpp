# include <Siv3D.hpp> // OpenSiv3D v0.6.16
#include "AutoTile.h"

/// @brief オートタイルの接続情報
struct AutoTileConnectivity
{
	bool connected[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
};

/// @brief オブジェクト上でカーソルを描きます。(タイルモード時)
void DrawTileCursor()
{
	Cursor::RequestStyle(CursorStyle::Hidden);
	const Vec2 cursorPos = Cursor::PosF();
	const Triangle triangle{ cursorPos, (cursorPos + Vec2{ 20, 6 }), (cursorPos + Vec2{ 6, 20 }) };
	triangle.stretched(1.2).movedBy(0.0, 0.5).draw(ColorF{ 0.25 });
	triangle.draw();
}

/// @brief オブジェクト上でカーソルを描きます。(ギミックモード時)
/// @param pos 基本座標
/// @param gimmikTexture 対象のテクスチャ
/// @param isFitFrame 格子状に納めるか否か
void DrawGimmikCursor(Vec2 pos, Texture gimmikTexture,bool isFitFrame)
{
	if(isFitFrame)gimmikTexture(0,0,32,32).drawAt(pos.x + 16,pos.y + 16);
	else          gimmikTexture(0,0,32,32).drawAt(pos.x,pos.y);
}

/// @brief マウスカーソルがあるタイルのインデックスを返します。
/// @param size オブジェクト数
/// @param tileSize タイルのサイズ（ピクセル）
/// @param offset タイルの描画位置のオフセット
/// @return タイルのインデックス。オブジェクト上にカーソルがない場合は none
[[nodiscard]]
Optional<Point> GetCursorIndex(const Size& size, int32 tileSize, const Point& offset)
{
	const Point cursorPos = (Cursor::Pos() - offset);

	if ((not InRange(cursorPos.x, 0, (size.x * tileSize - 1)))
		|| (not InRange(cursorPos.y, 0, (size.y * tileSize - 1))))
	{
		return none;
	}

	return{ cursorPos / tileSize };
}

int32 GetAutoTileIndexById(const Array<AutoTile> tiles, int32 id) {
	for (size_t i = 0; i < tiles.size(); i++) {
		if (tiles[i].getTileId() == id) {
			return i;
		}
	}
	return 0;
}

int32 GetSurroundingTileId(const Grid<uint32>& grid, Size size, int32 x, int32 y, int32 direction) {
	int32 id = 0;

	switch (direction) {
	case 0: // 上
		if (y - 1 < 0) break;
		id = grid[y - 1][x];
		break;
	case 1: // 下
		if (y + 1 > size.y - 1) break;
		id = grid[y + 1][x];
		break;
	case 2: // 左
		if (x - 1 < 0) break;
		id = grid[y][x - 1];
		break;
	case 3: // 右
		if (x + 1 > size.x - 1) break;
		id = grid[y][x + 1];
		break;
	default:
		return 0;
		break;
	}

	return id;
}

// マップサイズ（GUIで変更可能）
Size mapSize(25, 19);
TextEditState mapWidthText;
TextEditState mapHeightText;
Size gridSize(mapSize);

// マップデータ（動的に生成される）
Grid<uint32> grid(gridSize, 0);
Grid<AutoTileConnectivity> connectivityGrid(gridSize);

//保存ファイル名
TextEditState saveFileName;

//メインのアクションプロジェクトまでのパス
FilePath mainActionProjectPath = U"../../../I wanna Siv3D/OpenSiv3D_0.6.161/App/MapData/";


void Main()
{
	Window::SetTitle(U"I wanna Map Editor");
	Window::Resize(1280, 720);
	Scene::SetBackground(ColorF{ 0.75 });

	// オブジェクト画像の読み込み
	const FilePath objectPath = U"texture/object/";
	const Texture baseTexture{ objectPath + U"none.png" };
	const Array<AutoTile> autoTiles =
	{
		AutoTile{ Image{ objectPath + U"sprBlock.png" }, 1 },
		AutoTile{ Image{ objectPath + U"sprFloor.png" }, 2 },
		AutoTile{ Image{ objectPath + U"sprFloor2.png" }, 3 },
		AutoTile{ Image{ objectPath + U"sprFloor3.png" }, 4 },
		AutoTile{ Image{ objectPath + U"sprFloor4.png" }, 5 },
		AutoTile{ Image{ objectPath + U"sprFloor5.png" }, 6 },
		AutoTile{ Image{ objectPath + U"sprFloor6.png" }, 7 },
		AutoTile{ Image{ objectPath + U"sprSpikeUp.png" }, 21 },
		AutoTile{ Image{ objectPath + U"sprSpikeLeft.png" }, 22 },
		AutoTile{ Image{ objectPath + U"sprSpikeDown.png" }, 23 },
		AutoTile{ Image{ objectPath + U"sprSpikeRight.png" }, 24 },
		AutoTile{ Image{ objectPath + U"sprSave.png" }, 25 },
		AutoTile{ Image{ objectPath + U"sprBlockHide.png" }, 26 },
		AutoTile{ Image{ objectPath + U"sprBlockShootTrough.png" }, 27 },
		AutoTile{ Image{ objectPath + U"sprBlockFake.png" }, 28 },
	};
	size_t autoTileIndex = 0;

	// 敵の名前配列（ListBox と一致させる）
	const Array<String> gimmikNames = { U"罠針_上", U"罠針_左", U"罠針_下",U"罠針_右", U"罠トリガー", U"罠りんご",U"罠ブロック"};

	//敵の準備
	ListBoxState listBoxGimmiks{
		{
			gimmikNames
		}
	};
	listBoxGimmiks.selectedItemIndex = 0;

	ListBoxState listBoxPlacedGimmiks;
	listBoxPlacedGimmiks.selectedItemIndex = 0;

	const FilePath gimmikPath = U"texture/gimmik/";
	const Array<Texture> gimmikTextures =
	{
		Texture{ gimmikPath + U"spikeTrapUp.png" },
		Texture{ gimmikPath + U"spikeTrapLeft.png" },
		Texture{ gimmikPath + U"spikeTrapDown.png" },
		Texture{ gimmikPath + U"spikeTrapRight.png" },
		Texture{ gimmikPath + U"trapTrigger.png" },
		Texture{ gimmikPath + U"trapCherry.png" },
		Texture{ gimmikPath + U"trapBlock.png" },
	};

	// --- 敵情報構造体 ---
	struct GimmikInfo
	{
		String name;
		Vec2 pos;      // マップ上のピクセル座標（左上原点）
		int32 value1;  // 罠 : トリガーと結び付けるID
		double value2; // 罠 : 罠の向き
		double value3; // 罠 : 罠の速度
	};

	// 配置済みの敵リスト
	Array<GimmikInfo> placedGimmiks;

	// プレイヤー開始位置（固定にしてるが GUI で変更したければ TextEditState 等にする）
	Vec2 startPlayerPos = Vec2{ 30.0, 30.0 };

	// テキストボックスの初期文字列
	String posXText = Format(startPlayerPos.x);
	String posYText = Format(startPlayerPos.y);

	// テキストボックス
	TextEditState editPlayerX;
	TextEditState editPlayerY;
	editPlayerX.text = posXText;
	editPlayerY.text = posYText;

	// 敵の値（GUI で変更可能にするためのテキスト入力）
	TextEditState gimmikValueText1;
	TextEditState gimmikValueText2;
	TextEditState gimmikValueText3;

	// マップのセルの数
	 Size GridSize{ 25, 19 };

	// 選択オブジェクト欄のサイズ
	constexpr Size TileSize{ 5,9 };

	// マップを描画するときのオフセット
	constexpr Point LayerOffset{ 40, 40 };

	// オブジェクト選択欄のオフセット
	constexpr Point TileSelectOffset{ 900, 140 };

	// オートタイルの有無を格納する二次元配列
	Grid<uint32> grid(GridSize, 0);

	Grid<uint32> tileGrid(TileSize, 0);

	// オートタイルの接続情報を格納する二次元配列
	Grid<AutoTileConnectivity> connectivityGrid(GridSize);

	// 選択されているタイルのインデックス(左)
	Optional<Point> selectedTileIndex;
	// 選択されているタイルのインデックス(右)
	Optional<Point> selectedTileCursorIndex = Point{0,0};
	// 選択されているオブジェクトID
	int32 selectedTileId = 1;
	// 選択されているオブジェクトIDがautoTilesの何番目か
	int32 selectedTileNumberInAutoTiles = 0;

	// マップサイズ入力欄の初期テキスト
	mapWidthText.text = Format(mapSize.x);
	mapHeightText.text = Format(mapSize.y);

	// 横スクロール位置（0 ～ mapSize.x-25）
	double scrollX = 0;
	double scrollY = 0;

	// 設置物選択GUI用変数
	size_t settingMode = 0;//0: tile, 1: gimmik

	// 敵の値初期文字列
	gimmikValueText1.text = U"0";
	gimmikValueText2.text = U"0";
	gimmikValueText3.text = U"0";

	while (System::Update())
	{
		if (KeyC.down()) {
			ClearPrint();
		}

		//フォント宣言
		FontAsset::Register(U"Font", 20, Typeface::Regular);

		// 背景の市松模様を描く
		for (int32 y = 0; y < (Scene::Height() / 20); ++y)
		{
			for (int32 x = 0; x < (Scene::Width() / 20); ++x)
			{
				if (IsEven(y + x))
				{
					Rect{ (x * 20), (y * 20), 20 }.draw(ColorF{ 0.7 });
				}
			}
		}

		Rect(870, 0, 800, 800).draw(ColorF{ 0.1,0.5,0.1, 1.0 });


		// === 設置物選択GUI ===
		SimpleGUI::RadioButtons(settingMode, { U"ブロック", U"ギミック" }, Vec2{ 900, 40 });

		// === マップサイズ入力GUI ===
		FontAsset(U"Font")(U"マップの大きさ\nwidth        height").draw(1080, 10);
		SimpleGUI::TextBox(mapWidthText, Vec2{ 1080, 70}, 80);
		SimpleGUI::TextBox(mapHeightText, Vec2{ 1180,70}, 80);

		// 安全に文字列を int32 に変換して Clamp を適用
		if (const auto optW = s3d::ParseIntOpt<int32>(mapWidthText.text))
		{
			mapSize.x = Clamp(*optW, 1, 200);
		}
		// 同様に高さ
		if (const auto optH = s3d::ParseIntOpt<int32>(mapHeightText.text))
		{
			mapSize.y = Clamp(*optH, 1, 200);
		}

		// サイズが変更されたらグリッドを再生成
		if (gridSize.x != mapSize.x || gridSize.y != mapSize.y)
		{
			gridSize = Size{ mapSize.x, mapSize.y };
			grid = Grid<uint32>(gridSize, 0);
			connectivityGrid = Grid<AutoTileConnectivity>(gridSize);
		}  
        
		// === 横スクロールバー ===

		const int viewWidth = 25; // 表示するオブジェクト数（固定）
		const int viewHeight = 19;

		// 描画開始 X（スクロール反映）
		int startX = scrollX;
		int startY = scrollY;
		int endX = Min((int)scrollX + viewWidth, mapSize.x);
		int endY = Min((int)scrollY + viewHeight, mapSize.y);
		
		// mapSize.x が 25 以上の場合のみ表示
		if (mapSize.x > viewWidth)
		{
			SimpleGUI::Slider(scrollX, 0, (mapSize.x - viewWidth), Vec2{ 40, 660 }, 800, 120);
		}
		else
		{
			scrollX = 0; // 小さい場合はスクロール禁止
		}

		// mapSize.y が 19 以上の場合のみ表示
		if (mapSize.y > viewHeight)
		{
			SimpleGUI::VerticalSlider(scrollY, 0, (mapSize.y - viewHeight), Vec2{ 0, 40 }, 608, 120);
		}
		else
		{
			scrollY = 0; // 小さい場合はスクロール禁止
		}

		// === マップ保存ボタン & ファイル名指定text box ===
		FontAsset(U"Font")(U"保存ファイル名").draw(1080, 120);
		SimpleGUI::TextBox(saveFileName, Vec2{ 1080, 150 }, 150);

		FontAsset(U"Font")(U"CSV").draw(1080, 190);
		// CSV 保存処理
		if (SimpleGUI::Button(U"Save", Vec2{ 1080, 220 }))
		{
			CSV csv;
			const FilePath path = mainActionProjectPath + saveFileName.text + U".csv";
			if (!path.isEmpty())
			{
				for (int y = 0; y < grid.height(); ++y)
				{
					for (int x = 0; x < grid.width(); ++x)
					{
						csv.write(grid[y][x]);
					}
					csv.newLine();
				}
				csv.save(path);
				Print << U"保存しました: " << path;
			}
		}

		// CSV 読み込み処理
		if (SimpleGUI::Button(U"Load", Vec2{ 1170, 220 }))
		{
			const FilePath path = mainActionProjectPath + saveFileName.text + U".csv";

			if (FileSystem::Exists(path))
			{
				CSV csv(path);

				for (int y = 0; y < Min(grid.height(), csv.rows()); ++y)
				{
					for (int x = 0; x < Min(grid.width(), csv.columns(y)); ++x)
					{
						grid[y][x] = Parse<int32>(csv[y][x]);
					}
				}

				Print << U"CSVを読み込みました: " << path;
			}
			else
			{
				Print << U"ファイルが存在しません";
			}
		}


		FontAsset(U"Font")(U"JSON").draw(1080, 260);
		// JSON 保存処理
		if (SimpleGUI::Button(U"Save", Vec2{ 1080, 290 }))
		{
			// トップレベル配列 JSON を作る
			JSON json;

			JSON root;  // 配列に入れるオブジェクト

			const FilePath path = mainActionProjectPath + saveFileName.text + U".json";

			// --- startPlayerPos ---
			root[U"startPlayerPos"].push_back(startPlayerPos.x);
			root[U"startPlayerPos"].push_back(startPlayerPos.y);

			// --- Gimmiks ---
			for (const auto& e : placedGimmiks)
			{
				JSON gimmik;

				gimmik[U"gimmikName"] = e.name;

				gimmik[U"gimmikPos"].push_back(e.pos.x);
				gimmik[U"gimmikPos"].push_back(e.pos.y);

				gimmik[U"value1"] = e.value1;
				gimmik[U"value2"] = e.value2;
				gimmik[U"value3"] = e.value3;

				root[U"Gimmiks"].push_back(gimmik);
			}

			// root を配列に追加
			json.push_back(root);

			// 保存
			json.save(path);
			Print << U"情報を保存しました: " << path;
		}

		// JSON 読み込み処理
		if (SimpleGUI::Button(U"Load", Vec2{ 1170, 290 }))
		{
			const FilePath path = mainActionProjectPath + saveFileName.text + U".json";

			if (FileSystem::Exists(path))
			{
				JSON json = JSON::Load(path);

				if (json.isArray() && !json.isEmpty())
				{
					const JSON& root = json[0];

					// --- startPlayerPos ---
					if (root.contains(U"startPlayerPos"))
					{
						startPlayerPos.x = root[U"startPlayerPos"][0].get<double>();
						startPlayerPos.y = root[U"startPlayerPos"][1].get<double>();

						editPlayerX.text = Format(startPlayerPos.x);
						editPlayerY.text = Format(startPlayerPos.y);
					}

					// --- Gimmiks ---
					placedGimmiks.clear();

					if (root.contains(U"Gimmiks"))
					{
						for (const auto& g : root[U"Gimmiks"].arrayView())
						{
							GimmikInfo e;

							e.name = g[U"gimmikName"].getString();

							e.pos.x = g[U"gimmikPos"][0].get<double>();
							e.pos.y = g[U"gimmikPos"][1].get<double>();

							e.value1 = g[U"value1"].get<int32>();
							e.value2 = g[U"value2"].get<double>();
							e.value3 = g[U"value3"].get<double>();

							placedGimmiks << e;
							listBoxPlacedGimmiks.items.push_back(e.name);
						}
					}

					Print << U"JSONを読み込みました: " << path;
				}
			}
			else
			{
				Print << U"ファイルが存在しません";
			}
		}



		// 現在のオートタイルの種類
		const auto& autoTile = autoTiles[autoTileIndex];
		const int32 tileSize = autoTile.getTileSize();

		// カーソルでマウスオーバーしているタイルのインデックス
		auto cursorIndex = GetCursorIndex(Size{ viewWidth, mapSize.y }, tileSize, LayerOffset);

		if (cursorIndex)
		{
			// 実際のマップ座標へ補正
			cursorIndex->x += scrollX;
			cursorIndex->y += scrollY;

			// 範囲外なら無効化
			if (cursorIndex->x >= mapSize.x || cursorIndex->y >= mapSize.y)
				cursorIndex = none;
		}

		const auto touchedTilCcursorIndex = GetCursorIndex(tileGrid.size(), tileSize, TileSelectOffset);

		// クリック時の配置処理
		if (settingMode == 0) {

			// クリックでタイルを編集する
			if (cursorIndex && (MouseL.pressed() || MouseR.pressed()))
			{
				if (MouseL.pressed())
				{
					grid[*cursorIndex] = selectedTileId;
				}
				else
				{
					grid[*cursorIndex] = 0;
					connectivityGrid[*cursorIndex] = AutoTileConnectivity{};
				}

				selectedTileIndex = *cursorIndex;
			}
		} // --- 敵配置モードで左クリックしたら敵を配置 ---
		else if (settingMode == 1 && MouseL.down()) {
				// カーソルがマップ描画領域上にあるかを判定（GetCursorIndex を再利用）
				// viewWidth と startX は既に計算済みの前提
				const int viewWidth = 25; // 既定値と同じにしておく
				const auto maybeIndex = GetCursorIndex(Size{ viewWidth, mapSize.y }, tileSize, LayerOffset);
				if (maybeIndex){
					// world（マップ全体に対するピクセル座標）を計算する
					// 描画は ( (tileX - startX) * tileSize + LayerOffset.x ) なので逆算する
					const Vec2 cursorF = Cursor::PosF();
					// startX はスクロールから計算している描画開始オブジェクト
					// worldX = (cursorF.x - LayerOffset.x) + startX * tileSize
					Point worldPos{
						(cursorIndex->x - startX) * tileSize,
						(cursorIndex->y - startY) * tileSize
					};
					
					// 罠の値をパース（失敗時は 0.0 をデフォルトに）
					double val = 0.0;
					if (const auto d = Parse<int32>(gimmikValueText1.text)) val = d;

					GimmikInfo e;
					e.name = gimmikNames[*listBoxGimmiks.selectedItemIndex]; // listBox の選択と同期
					e.pos = Vec2{ worldPos };
					if (e.name == U"罠トリガー") {
						e.value1 = 0;
						e.value2 = 1.0;
						e.value3 = 1.0;
					}
					else {
						e.value1 = val;
						e.value2 = val;
						e.value3 = val;
					}

					placedGimmiks.push_back(e);
					listBoxPlacedGimmiks.items.push_back(e.name);
					Print << U"敵を追加: " << e.name << U" at " << e.pos << U" value=" << val;
				}
			}


		// マップチップを描く
		for (int32 y = 0; y < grid.height(); ++y)
		{
			for (int32 x = 0; x < grid.width(); ++x)
			{
				// 0〜24 の表示範囲から外れたら描画しない
				if (not InRange(x, startX, endX - 1) || not InRange(y, startY, endY - 1))
				{
					continue;
				}

				// 画面上の描画位置（x をスクロール補正）
				Point drawPos = Point{ (x - startX), y - startY } *tileSize + LayerOffset;
				//入力が無い場合、透明タイルを描画する
				baseTexture.draw(drawPos);

				if (grid[y][x]) {
					//描画対象のタイルを取得する
					const auto targetTile = autoTiles[GetAutoTileIndexById(autoTiles, grid[y][x])];
					int32 targetId = targetTile.getTileId();

					targetTile.getTile(targetId,0).draw(drawPos, settingMode == 0 ? ColorF{ 1.0, 1.0 } : ColorF{ 1.0, 0.5 });
				}
				else
				{
					Rect{ drawPos, tileSize }.drawFrame(1, ColorF{ 0.5 });
				}
			}
		}

		// ----- 盤面へのギミックの描画 -----

		int placedgimmikCount = 0;
		for (const auto& e : placedGimmiks){
			// e.name に対応する gimmikNames のインデックスを探す（存在しなければ -1）
			int idx = -1;
			for (int i = 0; i < static_cast<int>(gimmikNames.size()); ++i)
			{
				if (gimmikNames[i] == e.name)
				{
					idx = i;
					break;
				}
			}

			if (0 <= idx && idx < static_cast<int>(gimmikTextures.size())){
				Vec2 drawgimmikPos = Vec2{
					(e.pos.x - (scrollX * tileSize)) + LayerOffset.x,
					(e.pos.y - (scrollY * tileSize)) + LayerOffset.y
				};
				if (0 <= drawgimmikPos.x && drawgimmikPos.x < 800) {
					if (e.name == U"罠トリガー") {
						gimmikTextures[idx](0, 0, 32, 32).scaled({e.value2,e.value3}).draw(drawgimmikPos.x, drawgimmikPos.y).drawFrame(
							1.0, placedgimmikCount == listBoxPlacedGimmiks.selectedItemIndex ? ColorF(1.0, 0.0, 0.0, 1.0) : ColorF(0.0, 0.0));
					}
					else if (e.name == U"罠りんご") {// 格子状の交点に描画させたいテクスチャ
						gimmikTextures[idx](0, 0, 32, 32).drawAt(drawgimmikPos.x, drawgimmikPos.y).drawFrame(
							1.0, placedgimmikCount == listBoxPlacedGimmiks.selectedItemIndex ? ColorF(1.0, 0.0, 0.0, 1.0) : ColorF(0.0, 0.0));
					}
					else {//針など32*32の範囲に収まるテクスチャ
						gimmikTextures[idx](0, 0, 32, 32).draw(drawgimmikPos.x, drawgimmikPos.y).drawFrame(
							1.0, placedgimmikCount == listBoxPlacedGimmiks.selectedItemIndex ? ColorF(1.0, 0.0, 0.0, 1.0) : ColorF(0.0, 0.0));
					}
				}
			}
			else{
				// 名前が一致しないなら簡易に円で表す
				Circle{ e.pos, 8 }.draw(ColorF{ 1.0, 0.2 });
			}
			placedgimmikCount++;
		}


		// カーソルが重なっているタイルを強調表示する
		// マップタイル強調表示（scrollX 補正済）
		if (cursorIndex)
		{
			// 表示用の X 座標は (cursorIndex->x - startX)
			Point highlightPos{
				(cursorIndex->x - startX) * tileSize,
				(cursorIndex->y - startY) * tileSize
			};
			highlightPos += LayerOffset;

			switch (settingMode) {
				case 0: // オブジェクトモードのときはオレンジで強調
					Rect{ highlightPos, tileSize }.draw(ColorF{ 1.0, 0.5, 0.0, 0.5 });
					DrawTileCursor();
					break;
				case 1: // ギミックモードのときはそれ自体を描画
					DrawGimmikCursor(highlightPos, gimmikTextures[*listBoxGimmiks.selectedItemIndex], gimmikNames[*listBoxGimmiks.selectedItemIndex] != U"罠りんご");
					break;
			}
		}

		int32 autoTilesValue = 0;
		int32 tileCounter = 1;

		// 設置物選択GUIの描画

		// 座標の固定値
		const Vec2 baseJsonValueUIPos = { 1100,580 };

		if (settingMode == 0) {// オブジェクト設置モードのとき
			// オブジェクト選択欄を描画する
			for (int32 y = 0; y < tileGrid.height(); ++y) {
				for (int32 x = 0; x < tileGrid.width(); ++x) {
					if (autoTiles[autoTilesValue].getTileId() == tileCounter) {
						autoTiles[autoTilesValue].getTile(tileCounter, 0).draw(Point{ x, y } *tileSize + TileSelectOffset);
						if (autoTilesValue != autoTiles.size() - 1) autoTilesValue++;
					}
					Rect{ (Point{ x, y } *tileSize + TileSelectOffset), tileSize }.drawFrame(1, ColorF{ 1.0 });
					tileCounter++;
				}
			}

			// オブジェクト選択欄のカーソルが重なっているタイルを強調表示する
			if (touchedTilCcursorIndex)
			{
				Rect{ (*touchedTilCcursorIndex * tileSize + TileSelectOffset), tileSize }.draw(ColorF{ 1.0, 0.5, 0.0, 0.0 }).drawFrame(2, Palette::Blue);
				DrawTileCursor();
			}

			// オブジェクト選択欄でクリックされたら、そのタイルを選択する
			if (touchedTilCcursorIndex && MouseL.down())
			{
				selectedTileCursorIndex = touchedTilCcursorIndex;
				selectedTileId = (touchedTilCcursorIndex->y * tileGrid.width() + touchedTilCcursorIndex->x + 1);

				bool isFound = false;// autoTiles内で選択されたオブジェクトIDが存在するかを確認
				for (size_t i = 0; i < autoTiles.size(); i++) {
					if (autoTiles[i].getTileId() == selectedTileId) {
						isFound = true;
					}
				}

				if (!isFound) selectedTileId = 0;//存在しない場合は透明タイルを選択
			}

			// 選択されているタイルを赤で強調表示する
			Rect{ (*selectedTileCursorIndex * tileSize + TileSelectOffset), tileSize }.draw(ColorF{ 1.0, 0.5, 0.0, 0.0 }).drawFrame(2, Palette::Red);
		}
		else {// 敵設置モードのとき
			SimpleGUI::ListBox(listBoxGimmiks, Vec2{ 900, 140 }, 120, 156);
			SimpleGUI::ListBox(listBoxPlacedGimmiks, Vec2{ 900, 350 }, 120, 300);

			if (listBoxPlacedGimmiks.selectedItemIndex) {
				const size_t idx = static_cast<size_t>(*listBoxPlacedGimmiks.selectedItemIndex);

				if (SimpleGUI::Button(U"Delete", Vec2{ baseJsonValueUIPos.x, 350 })) {
					placedGimmiks.remove_at(idx);
					listBoxPlacedGimmiks.items.remove_at(idx);
					continue;
				}

				// === id入力GUI ===
				if (placedGimmiks[idx].name == U"罠針_上" || placedGimmiks[idx].name == U"罠針_左" || placedGimmiks[idx].name == U"罠針_下" || placedGimmiks[idx].name == U"罠針_右" || placedGimmiks[idx].name == U"罠りんご") {
					FontAsset(U"Font")(U"id : ").draw(baseJsonValueUIPos.x, 390);
					FontAsset(U"Font")(U"角度 : ").draw(baseJsonValueUIPos.x, 430);
					FontAsset(U"Font")(U"速度 : ").draw(baseJsonValueUIPos.x, 470);
				}
				else if (placedGimmiks[idx].name == U"罠トリガー") {
					FontAsset(U"Font")(U"id : ").draw(baseJsonValueUIPos.x, 390);
					FontAsset(U"Font")(U"x scale : ").draw(baseJsonValueUIPos.x, 430);
					FontAsset(U"Font")(U"y scale : ").draw(baseJsonValueUIPos.x, 470);
				}
				else if (placedGimmiks[idx].name == U"罠ブロック") {
					FontAsset(U"Font")(U"id : ").draw(baseJsonValueUIPos.x, 390);
					FontAsset(U"Font")(U"").draw(baseJsonValueUIPos.x, 430);
					FontAsset(U"Font")(U"").draw(baseJsonValueUIPos.x, 470);
				}

				static Optional<size_t> prevIdx = none;

				// idx が変わったときだけ text を初期化する
				if (prevIdx != idx)
				{
					gimmikValueText1.text = Format(placedGimmiks[idx].value1);
					gimmikValueText2.text = Format(placedGimmiks[idx].value2);
					gimmikValueText3.text = Format(placedGimmiks[idx].value3);
					prevIdx = idx;
				}

				SimpleGUI::TextBox(gimmikValueText1, Vec2{ baseJsonValueUIPos.x + 70, 390 }, 80);
				SimpleGUI::TextBox(gimmikValueText2, Vec2{ baseJsonValueUIPos.x + 70, 430 }, 80);
				SimpleGUI::TextBox(gimmikValueText3, Vec2{ baseJsonValueUIPos.x + 70, 470 }, 80);

				// 変更があった瞬間だけ value に反映
				if (gimmikValueText1.textChanged && gimmikValueText1.text != U"")
				{
					if (const auto evt = Parse<double>(gimmikValueText1.text))
					{
						// clamp range 例：0.0〜999.0 とか
						placedGimmiks[idx].value1 = Math::Max(0.0, evt);
					}
				}

				// 変更があった瞬間だけ value に反映
				if (gimmikValueText2.textChanged && gimmikValueText2.text != U"")
				{
					if (const auto evt = Parse<double>(gimmikValueText2.text))
					{
						// clamp range 例：0.0〜999.0 とか
						placedGimmiks[idx].value2 = Math::Max(0.0, evt);
					}
				}

				// 変更があった瞬間だけ value に反映
				if (gimmikValueText3.textChanged && gimmikValueText3.text != U"")
				{
					if (const auto evt = Parse<double>(gimmikValueText3.text))
					{
						// clamp range 例：0.0〜999.0 とか
						placedGimmiks[idx].value3 = Math::Max(0.0, evt);
					}
				}
			}

			// --- UI描画 ---

			FontAsset(U"Font")(U"Player初期座標").draw(baseJsonValueUIPos.x, baseJsonValueUIPos.y, Palette::White);
			FontAsset(U"Font")(U"x").draw(baseJsonValueUIPos.x, baseJsonValueUIPos.y + 40, Palette::White);
			FontAsset(U"Font")(U"y").draw(baseJsonValueUIPos.x, baseJsonValueUIPos.y + 80, Palette::White);

			// X テキストボックス
			if (SimpleGUI::TextBox(editPlayerX, Vec2{ baseJsonValueUIPos.x + 20, baseJsonValueUIPos.y + 40 }, 80))
			{
				// 入力されたとき、数値か判定
				if (editPlayerX.text)
				{
					startPlayerPos.x = Parse<int>(editPlayerX.text);
				}
			}

			// Y テキストボックス
			if (SimpleGUI::TextBox(editPlayerY, Vec2{ baseJsonValueUIPos.x + 20, baseJsonValueUIPos.y + 80 }, 80))
			{
				if (editPlayerY.text)
				{
					startPlayerPos.y = Parse<int>(editPlayerY.text);
				}
			}

			// プレイヤー開始位置を描画する
			Circle{ startPlayerPos - Vec2{ scrollX * tileSize, scrollY * tileSize } + Vec2{ LayerOffset.x, LayerOffset.y }, 10 }.draw(Palette::Yellow);
		}
	}
}
