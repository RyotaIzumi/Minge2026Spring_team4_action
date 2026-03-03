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

	// トラップの作動時間が指定時間に達したかどうかを取得
	bool SpecialTrap::reachedTrapTime(double time) {
		return trapStopwatch.s() >= time;
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
}
