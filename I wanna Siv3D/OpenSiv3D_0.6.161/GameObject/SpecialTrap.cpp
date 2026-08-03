#include "SpecialTrap.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	SpecialTrap::SpecialTrap(Vec2 startPos, int32 id) {
		//GameObject.hの値初期化
		pos = { startPos.x, startPos.y };
		type = ObjectType::SpecialTrap;
		canPlayerKill = false;
		trapID = id;
	}
	void SpecialTrap::update() {
		trapUpdate();
	}
	void SpecialTrap::trapUpdate() {
	}
	void SpecialTrap::draw() const {
		TextureAsset(textureName).scaled(textureScale).draw(pos);
		//hitBox->draw(Palette::Pink);
	}
	void SpecialTrap::onCollision(GameObject& other) {
	}

	// トラップを作動させる
	void SpecialTrap::specialTrapActivate() {
		isActivated = true;
	}
	// トラップが作動しているかどうかを取得
	bool SpecialTrap::getIsActivated() const {
		return isActivated;
	}
	// 罠のIDを取得
	int32 SpecialTrap::getTrapID() const {
		return trapID;
	}
	// 罠の現在IDを設定
	void SpecialTrap::setNowTrapID(int32 id) {
		nowTrapID = id;
	}

	// playerの死亡状態を設定
	void SpecialTrap::setIsPlayerDied(bool dead) {
		isPlayerDied = dead;
	}

	// トラップの作動時間が指定時間に達したかどうかを取得
	bool SpecialTrap::reachedTrapTime(double time) {
		return trapStopwatch.s() >= time;
	}

	//playerまでの角度を必要とするかどうかを取得
	bool SpecialTrap::getIsNeedPlayerDir() const{
		return isNeedPlayerDir;
	}

	//目標の座標から角度を計算
	void SpecialTrap::setDirection(Vec2 targetPos) {
		calculateDirection(pos,targetPos);
	}

	// ----- 偽警告ウィンドウトラップ -----
	WarningWindowTrap::WarningWindowTrap(Vec2 startPos, int32 id) : SpecialTrap(startPos,id) {
		textureName = U"warningTrap";
		textureScale = 0.0;
		textureAlpha = 0.0;
		textureSize = Vec2{ 480,230 };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{textureSize});
		hitBox->setPos(pos);//当たり判定の位置をテクスチャの中心に調整
		canPlayerKill = false;
	}

	void WarningWindowTrap::trapUpdate() {
		switch (trapStep) {
		case 0:
			if (trapID == nowTrapID) {
				canPlayerKill = true;
				isActivated = true;
				scaleTimer.restart();
				Global::warningTrapPaused = true;
				trapStep++;
			}
			break;
		case 1://テクスチャ出現演出
			textureScale = 0.7 + 0.1 * scaleTimer.progress0_1();
			textureAlpha = scaleTimer.progress0_1();
			if (scaleTimer.reachedZero()) {
				trapStopwatch.restart();
				AudioAsset(Sound::ERROR).playOneShot();
				trapStep++;
			}
			break;
		case 2://偽フリーズ時間
			if (reachedTrapTime(4.0)) {
				trapStopwatch.restart();
				Global::warningTrapPaused = false;
				trapStep++;
			}
			break;
		case 3://下移動までの待機時間
			if (reachedTrapTime(1.5)) {
				AudioAsset(Sound::CHERRYFALL).playOneShot();
				trapStep++;
			}
			break;
		case 4://下移動
			pos.y += 5;
			if (pos.y > Global::windowHeight + textureSize.y / 2)trapStep++;
			break;
		case 5://終了
				canPlayerKill = false;
				isActivated = false;
				textureScale = 0.0;
				textureAlpha = 0.0;
				pos.y = -10000;
		}

		hitBox->setPos(pos);
	}

	void WarningWindowTrap::draw() const {
		TextureAsset(textureName).scaled(textureScale).drawAt(pos.x,pos.y, ColorF(1.0, canPlayerKill ? textureAlpha : 0.0));
		//hitBox->draw(Palette::Pink);
	}

	// ----- 偽Steamトラップ -----
	SteamTrap::SteamTrap(Vec2 startPos, int32 id) : SpecialTrap(startPos, id) {
		textureName = U"steamTrap";
		textureScale = 1.0;
		textureAlpha = 1.0;
		textureSize = Vec2{ 224,77 };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ textureSize });
		hitBox->setPos(pos);//当たり判定の位置をテクスチャの中心に調整
		canPlayerKill = false;
		trapStep = 0;
		basePos = pos;
	}

	void SteamTrap::trapUpdate() {
		switch (trapStep) {
		case 0:
			if (trapID == nowTrapID) {
				canPlayerKill = true;
				isActivated = true;
				moveTimer.restart();
				trapStep++;
			}
			break;
		case 1://上昇
			pos.y = basePos.y - moveRange * moveTimer.progress0_1();
			if (moveTimer.reachedZero()) {
				basePos = pos;
				trapStopwatch.restart();
				trapStep++;
			}
			break;
		case 2://待機
			if (reachedTrapTime(3.0)) {
				moveTimer.restart();
				trapStep++;
			}
			break;
		case 3://下降
			pos.y = basePos.y + moveRange * moveTimer.progress0_1();
			if (moveTimer.reachedZero()) {
				basePos = pos;
				trapStep++;
			}
			break;
		case 4://終了
			pos.y = -10000;
			canPlayerKill = false;
			break;
		}

		hitBox->setPos(pos);
	}

	void SteamTrap::draw() const {
		TextureAsset(textureName).scaled(textureScale).drawAt(pos);
		//hitBox->draw(Palette::Pink);
	}

	// ----- ツリートラップ -----
	TreeTrap::TreeTrap(Vec2 startPos, int32 id, String name) : SpecialTrap(startPos, id) {
		textureName = name;
		textureScale = 1.0;
		textureAlpha = 1.0;
		
		hitboxPos = {pos.x,pos.y - 32};
		hitBox = std::make_shared<CircleHitBox>(hitboxPos, 50);
		hitBox->setPos(hitboxPos);//当たり判定の位置をテクスチャの中心に調整
		canPlayerKill = false;
		trapStep = 0;
		basePos = pos;
	}

	void TreeTrap::trapUpdate() {
		switch (trapStep) {
		case 0:
			if (trapID == nowTrapID) {
				canPlayerKill = true;
				isActivated = true;
				AudioAsset(Sound::SPIKETRAP).playOneShot();

				moveTimer.restart();
				trapStep++;
			}
			break;
		case 1://下降
			pos.y = basePos.y + moveRange * moveTimer.progress0_1();
			if (moveTimer.reachedZero()) {
				basePos = pos;
				trapStep++;
			}
			break;
		case 2://終了
			pos.y = -10000;
			canPlayerKill = false;
			break;
		}

		hitboxPos = { pos.x,pos.y - 32 };
		hitBox->setPos(hitboxPos);
	}

	void TreeTrap::draw() const {
		TextureAsset(textureName).scaled(textureScale).drawAt(pos);
		//hitBox->draw(Palette::Pink);
	}

	// ----- タイトルトラップ -----
	TitleTrap::TitleTrap(Vec2 startPos, int32 id) : SpecialTrap(startPos, id) {
		textureName = U"";
		textureScale = 1.0;
		textureAlpha = 1.0;
		textureSize = Vec2{ 70,16 };

		hitBox = std::make_shared<RectHitBox>(pos, textureSize);
		hitBox->setPos(pos);//当たり判定の位置をテクスチャの中心に調整
		canPlayerKill = false;
		trapStep = 0;
		basePos = pos;
	}

	void TitleTrap::trapUpdate() {
		switch (trapStep) {
		case 0:
			if (trapID == nowTrapID) {
				Window::SetTitle(U"TestPlayGame                       ");

				canPlayerKill = true;
				isActivated = true;
				AudioAsset(Sound::SPIKETRAP).playOneShot();

				moveTimer.restart();
				trapStep++;
			}
			break;
		case 1://下降
			pos.y = basePos.y + moveRange * moveTimer.progress0_1();
			if (moveTimer.reachedZero()) {
				basePos = pos;
				trapStep++;
			}
			break;
		case 2://終了
			pos.y = -10000;
			canPlayerKill = false;
			break;
		}

		hitBox->setPos(pos);
	}

	void TitleTrap::draw() const {
		FontAsset(U"TitleTrap")(U"(Debug Build)").drawAt(pos,ColorF(Palette::Black));
		//hitBox->draw(Palette::Pink);
	}

	// ----- タイトルトラップ その2 -----
	TitleTrap2::TitleTrap2(Vec2 startPos, int32 id) : SpecialTrap(startPos, id) {
		textureName = U"";
		textureScale = 1.0;
		textureAlpha = 1.0;
		textureSize = Vec2{ 70,16 };

		hitBox = std::make_shared<RectHitBox>(pos, textureSize);
		hitBox->setPos(pos);//当たり判定の位置をテクスチャの中心に調整
		canPlayerKill = false;
		trapStep = 0;
		basePos = pos;
	}

	void TitleTrap2::trapUpdate() {
		switch (trapStep) {
		case 0:
			if (trapID == nowTrapID) {
				Window::SetTitle(U"☻                                          ");

				canPlayerKill = true;
				isActivated = true;
				AudioAsset(Sound::SPIKETRAP).playOneShot();

				moveTimer.restart();
				trapStep++;
			}
			break;
		case 1://下降
			pos.y = basePos.y + moveRange * moveTimer.progress0_1();
			if (moveTimer.reachedZero()) {
				basePos = pos;
				trapStep++;
			}
			break;
		case 2://終了
			pos.y = -10000;
			canPlayerKill = false;
			break;
		}

		hitBox->setPos(pos);
	}

	void TitleTrap2::draw() const {
		FontAsset(U"TitleTrap")(U"TestPlayGame").drawAt(pos, ColorF(Palette::Black));
		//hitBox->draw(Palette::Pink);
	}

	// ----- 広告ウィンドウトラップ -----
	AdWindowTrap::AdWindowTrap(Vec2 startPos, int32 id) : SpecialTrap(startPos, id) {
		textureName = U"adTrap";
		textureScale = 0.0;
		textureAlpha = 0.0;
		textureSize = Vec2{ 256,320 };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ textureSize });
		hitBox->setPos(pos);//当たり判定の位置をテクスチャの中心に調整
		canPlayerKill = false;
	}

	void AdWindowTrap::trapUpdate() {
		switch (trapStep) {
		case 0:
			if (trapID == nowTrapID) {
				isActivated = true;
				AudioAsset(Sound::VC_BIKKURI).playOneShot();
				scaleTimer.restart();
				trapStep++;
			}
			break;
		case 1://テクスチャ出現演出
			textureScale = 0.7 + 0.3 * scaleTimer.progress0_1();
			textureAlpha = scaleTimer.progress0_1();
			if (scaleTimer.reachedZero()) {
				//4つの地点からランダムで座標を決定
				Array<Vec2>randomPos = { {596,307} };
				deleteButtonPos = randomPos.choice();

				trapStep++;
			}
			break;
		case 2://xを押すまで出現
			if (SimpleGUI::ButtonAt(U"x",deleteButtonPos,20,true)) {
				scaleTimer.restart();
				deleteButtonPos = {-1000,-1000};
				isShowHideLine = true;
				trapStep++;
			}
			break;
		case 3://windowを消す
			textureScale = 1.0 - 0.3 * scaleTimer.progress0_1();
			textureAlpha = scaleTimer.progress1_0();
			if (scaleTimer.reachedZero()) {
				trapStep++;
			}
			break;
		case 4:
			if (nowTrapID == 12) {
				isShowHideLine = false;
			}
			break;
		case 5://終了
			canPlayerKill = false;
			isActivated = false;
			textureScale = 0.0;
			textureAlpha = 0.0;
			pos.y = -10000;
		}

		holeText = isShowHideLine ? U"忘れてて草" : U"wwwwwwwww";

		hitBox->setPos(pos);
	}

	void AdWindowTrap::draw() const {
		//床隠し
		if (isShowHideLine) TextureAsset(U"hideLine").draw(352,576);

		TextureAsset(textureName).scaled(textureScale).drawAt(pos, ColorF(1.0, isActivated ? textureAlpha : 0.0));
		//hitBox->draw(Palette::Pink);

		FontAsset(U"Big")(holeText).drawAt({400,1100},ColorF(Palette::Black));

		SimpleGUI::ButtonAt(U"x", deleteButtonPos,20,false);
	}

	// ----- 偽Discordトラップ -----
	DiscordTrap::DiscordTrap(Vec2 startPos, int32 id) : SpecialTrap(startPos, id) {
		textureName = U"discordTrap";
		textureScale = 1.0;
		textureAlpha = 1.0;
		textureSize = Vec2{ 224,67 };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ textureSize });
		hitBox->setPos(pos);//当たり判定の位置をテクスチャの中心に調整
		canPlayerKill = true;
		isActivated = false;
		trapStep = 0;
		basePos = pos;
	}

	void DiscordTrap::trapUpdate() {
		switch (trapStep) {
		case 0:
			if (trapID == nowTrapID) {
				moveTimer.restart();
				isActivated = true;
				AudioAsset(Sound::DISCORD).playOneShot();
				trapStep++;
			}
			break;
		case 1://横移動
			pos.x = basePos.x - moveRange * EaseOutQuad(moveTimer.progress0_1());
			if (moveTimer.reachedZero()) {
				basePos = pos;
				trapStopwatch.restart();
				trapStep++;
			}
			break;
		case 2://待機
			if (reachedTrapTime(2.0) && !isPlayerDied) {
				moveTimer.restart();
				trapStep++;
			}
			break;
		case 3://もどる
			pos.x = basePos.x + moveRange * EaseInQuad(moveTimer.progress0_1());
			if (moveTimer.reachedZero()) {
				trapStep++;
			}
			break;
		case 4://終了
			canPlayerKill = false;
			isDelete = true;
			break;
		}

		hitBox->setPos(pos);
	}

	void DiscordTrap::draw() const {
		if(isActivated) TextureAsset(textureName).scaled(textureScale).drawAt(pos);
		//hitBox->draw(Palette::Pink);
	}

	// ----- 偽Discordについてるりんごトラップ -----
	DiscordCherryTrap::DiscordCherryTrap(Vec2 startPos, int32 id) : SpecialTrap(startPos, id) {
		textureName = U"sprCherryLow";
		textureScale = 1.0;
		textureAlpha = 1.0;
		gravity = 0.3;
		hitBox = std::make_shared<CircleHitBox>(pos, 10);
		hitBox->setPos(pos);//当たり判定の位置をテクスチャの中心に調整
		canPlayerKill = true;
		isActivated = false;
		trapStep = 0;
		basePos = pos;
	}

	void DiscordCherryTrap::trapUpdate() {
		switch (trapStep) {
		case 0:
			if (trapID == nowTrapID) {
				moveTimer.restart();
				isActivated = true;
				trapStep++;
			}
			break;
		case 1://横移動
			pos.x = basePos.x - moveRange * EaseOutQuad(moveTimer.progress0_1());
			if (moveTimer.reachedZero()) {
				basePos = pos;
				trapStopwatch.restart();
				trapStep++;
			}
			break;
		case 2://待機
			if (reachedTrapTime(1.2) && !isPlayerDied) {
				hspeed = -7;
				vspeed = 0;
				AudioAsset(Sound::BLOCKBREAK).playOneShot();
				trapStep++;
			}
			break;
		case 3://もどる
			vspeed += gravity;
			pos.x += hspeed;
			pos.y += vspeed;
			if (pos.y > 1000) {
				trapStep++;
			}
			break;
		case 4://終了
			canPlayerKill = false;
			isDelete = true;
			break;
		}

		hitBox->setPos(pos);
	}

	void DiscordCherryTrap::draw() const {
		if (isActivated) TextureAsset(textureName).scaled(textureScale).drawAt(pos);
		//hitBox->draw(Palette::Pink);
	}

	// ----- マウストラップ -----
	MouseTrap::MouseTrap(Vec2 startPos) : SpecialTrap(startPos, -1) {
		textureName = U"sprCherryLow";
		textureScale = 1.0;
		textureAlpha = 1.0;
		hitBox = std::make_shared<CircleHitBox>(pos, 2);
		hitBox->setPos(pos);//当たり判定の位置をテクスチャの中心に調整

		canPlayerKill = true;
		isActivated = false;
		isNeedPlayerDir = true;

		trapStopwatch.start();

		speed = 7;
		direction = 0;
	}

	void MouseTrap::trapUpdate() {

		if (Abs(Cursor::DeltaF().x) > 0.1 || Abs(Cursor::DeltaF().y) > 0.1) {
			trapStopwatch.restart();
		}

		if (reachedTrapTime(5.0) && Cursor::OnClientRect() && !isPlayerDied) {
			calculateSpeed();
			pos.x += hspeed;
			pos.y += vspeed;
			Cursor::SetPos(pos.asPoint());
		}
		else {
			pos = Cursor::PosF();
		}

		if (isPlayerDied) {
			Cursor::RequestStyle(U"cursorPiece");
		}

		hitBox->setPos(pos);
	}

	void MouseTrap::draw() const {
		//if (isActivated) TextureAsset(textureName).scaled(textureScale).drawAt(pos);
		//hitBox->draw(Palette::Pink);
	}

	// ----- PandDトラップ -----
	PanddTrap::PanddTrap(Vec2 startPos, int32 id) : SpecialTrap(startPos, id) {
		textureName = U"panddChanTrap";
		textureScale = 1.0;
		textureAlpha = 1.0;
		textureSize = Vec2{ 320,160 };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ textureSize });
		hitBox->setPos(pos);//当たり判定の位置をテクスチャの中心に調整
		canPlayerKill = false;
		isActivated = false;
		trapStep = 0;
		basePos = pos;
	}

	void PanddTrap::trapUpdate() {
		switch (trapStep) {
		case 0:
			if (trapID == nowTrapID) {
				canPlayerKill = true;
				isActivated = true;
				moveTimer.restart();
				trapStep++;
			}
			break;
		case 1://上昇
			pos.y = basePos.y - moveRange * moveTimer.progress0_1();
			if (moveTimer.reachedZero()) {
				basePos = pos;
				trapStep++;
			}
			break;
		case 2://終了

			break;
		}

		hitBox->setPos(pos);
	}

	void PanddTrap::draw() const {
		if(isActivated)
		TextureAsset(textureName).scaled(textureScale).drawAt(pos);
		//hitBox->draw(Palette::Pink);
	}
}
