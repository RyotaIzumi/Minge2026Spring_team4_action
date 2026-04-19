#include "ExBossCherry.h"
#include "../../Audio/AudioAsset.h"
#include "../../StageManager/BossStageManager.h"

namespace Iwanna {
	ExBossCherry::ExBossCherry(Vec2 startPos, double scale, BossStageManager& manager) : bossStageManager(&manager), Cherry(startPos, scale) {

		//GameObject.hの値初期化
		pos = startPos;
		scaleMag = scale;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);
		type = ObjectType::Cherry;
		cherryType = CherryType::ExBoss;

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = false;

		hasHp = true;
		maxHp = 25;
		hp = maxHp;

		baseCenterPos = Vec2{ 400, 350 };
		speed = 20;
		direction = 90;
		gravity = 0;

		c = 0;
		r = 20;
		attackIntervalTime = 3.5;
		startStep = 0;
	}

	void ExBossCherry::barrageUpdate() {
		//ボス戦開始時の処理
		switch (startStep) {
		case 0:
			break;
		case 1:
			break;
		case 2://浮遊処理
			pos.y = -r * sin(Math::ToRadians(c)) + baseCenterPos.y;
			c += 1;
			break;
		}
	}

	void ExBossCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLowBoss").scaled(scaleMag).drawAt(pos.x - 1, pos.y - 1, ColorF(1.0, isMuteki ? 0.6 : 1.0));
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}

	SordCherriesManager::SordCherriesManager(Vec2 startPos, double scale, BossStageManager& manager) : bossStageManager(&manager), Cherry(startPos, scale) {
		pos = startPos;
		canPlayerKill = false;
		isDeleteOutOfScreen = false;

		alpha = 1.0;

		hasAnimation = false;
		cherryTextureName = U"sprCherryLowWhite";

		depth = 28;

		cherryType = CherryType::ExBoss;
	}
}
