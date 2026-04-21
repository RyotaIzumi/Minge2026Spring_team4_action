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

		sordCherriesManager = bossStageManager->getExBossSordManagerCherry().get();

		canPlayerKill = true;
		isDelete = false;
		isOutOfScreen = false;
		isDeleteOutOfScreen = false;

		hasHp = true;
		maxHp = 25;
		hp = maxHp;

		baseCenterPos = Vec2{ 800, 330 };
		speed = 20;
		direction = 90;
		gravity = 0;

		depth = 11;

		c = 0;
		r = 20;
		attackIntervalTime = 3.5;
		startStep = 0;
	}

	void ExBossCherry::barrageUpdate() {
		//ボス戦開始時の処理
		switch (startStep) {
		case 0:
			if (pos.y < -300) {
				pos.x = 800;
				pos.y = -300;
				speed = 0;
				movePosition(Vec2{ 800, 330 }, 2.0, false);
				rotateDirection(140, 2.0, false);
				startStep++;
			}
			break;
		case 1:
			if (getIsMoveFinished() && getIsRotateFinished()) {
				movePosition(Vec2{ 800, 260 }, 1.0, false);//抜刀
				rotateDirection(180, 1.5, true);
				//sordCherriesManager->pos
				startStep++;
			}
			break;
		case 2:
			if (getIsMoveFinished()) {
				
				startStep++;
			}
			break;
		}
	}

	void ExBossCherry::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(U"sprCherryLowBoss").scaled(scaleMag).rotated(Math::ToRadians(textureAngle)).drawAt(pos.x - 1, pos.y - 1, ColorF(1.0, isMuteki ? 0.6 : 1.0));
		//hitBox->draw(ColorF(0.7,0.7));//判定の可視化
	}

	// --- 剣を構成するりんご --- //
	SordCherry::SordCherry(Vec2 startPos, double scale, CherryColorType colorType) : BarrageCherry(startPos, scale, colorType) {
		pos = startPos;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);

		canPlayerKill = false;
		isDeleteOutOfScreen = false;
		alpha = 1.0;
		hasAnimation = false;
		cherryTextureName = U"sprCherryLowAllWhite";
		depth = 18;

		alpha = 0.0;

		cherryColorType = colorType;
		setTypeColor();
	}

	void SordCherry::barrageUpdate() {
		switch (startStep) {
		case 0:
			r = calculateDistance(pos, sordBaseCenterPos);
			calculateDirection(pos, sordBaseCenterPos);
			c = direction;
			alpha = 1.0;
			
			startStep++;
			break;
		case 1:
			pos.x = r * cos(Math::ToRadians(c)) + sordBaseCenterPos.x;
			pos.y = -r * sin(Math::ToRadians(c)) + sordBaseCenterPos.y;
			break;
		}

		setTypeColor();
	}

	void SordCherry::setSordBaseCenterPos(Vec2 pos) {
		sordBaseCenterPos = pos;
	}

	// ----- 剣型りんごの動き制御用りんご ----- //
	SordCherriesManager::SordCherriesManager(Vec2 startPos, double scale, BossStageManager& manager) : bossStageManager(&manager), Cherry(startPos, scale) {
		pos = startPos;
		canPlayerKill = false;
		isDeleteOutOfScreen = false;

		alpha = 1.0;

		hasAnimation = false;
		cherryTextureName = U"sprCherryLowWhite";

		depth = 9;

		cherryType = CherryType::ExBoss;

		createSordCherries();
	}

	void SordCherriesManager::createSordCherries() {
		//ラムダ式で工場関数を定義
		//剣の刃を構成するりんご
		auto bladeCherryFactory = [this]() {
			return std::make_shared<SordCherry>(pos, 1.0, CherryColorType::Gray);
		};
		//剣の柄を構成するりんご
		auto handleCherryFactory = [this]() {
			return std::make_shared<SordCherry>(pos, 1.0, CherryColorType::Black);
		};
		//剣の柄を構成するりんご
		auto handleCherry2Factory = [this]() {
			return std::make_shared<SordCherry>(pos, 0.8, CherryColorType::Black);
		};

		// 刃部分の生成
		int bladeXnum = 2;
		int bladeYnum = 8;
		double bladeInterval = 16.0;
		Vec2 bladeStartPos = Vec2{ pos.x - (bladeInterval * (bladeXnum - 1) / 2), pos.y };
		for (int j = 0; j <= bladeYnum; j++) {
			for (int i = 0; i < bladeXnum; i++) {
				auto cherry = bladeCherryFactory();
				if (j < bladeYnum) {
					cherry->pos.x = bladeStartPos.x + i * bladeInterval;
					cherry->pos.y = bladeStartPos.y - j * bladeInterval;
					cherry->setSordBaseCenterPos(pos);
					sordCherries.push_back(cherry);
				} else {
					cherry->pos.x = pos.x;
					cherry->pos.y = bladeStartPos.y - j * bladeInterval;
					cherry->setSordBaseCenterPos(pos);
					sordCherries.push_back(cherry);
				}
			}
		}

		// 柄部分の生成
		int handleXnum = 2;
		int handleYnum = 7;
		double handleInterval = 10.0;
		Vec2 handleStartPos = Vec2{ pos.x - (handleInterval * (handleXnum - 1) / 2), pos.y };
		for (int j = 0; j < handleYnum; j++) {
			for (int i = 0; i < handleXnum; i++) {
				auto cherry = handleCherry2Factory();
				if (j < handleYnum) {
					cherry->pos.x = handleStartPos.x + i * handleInterval;
					cherry->pos.y = handleStartPos.y + j * handleInterval;
					cherry->setSordBaseCenterPos(pos);
					sordCherries.push_back(cherry);
				}
			}
		}

		// 柄部分(黒)の生成
		handleXnum = 6;
		handleYnum = 2;
		handleInterval = 14.0;
		handleStartPos = Vec2{ pos.x - (handleInterval * (handleXnum - 1) / 2), pos.y };
		for (int j = 0; j < handleYnum; j++) {
			for (int i = 0; i < handleXnum; i++) {
				auto cherry = handleCherryFactory();
				if (j < handleYnum) {
					cherry->pos.x = handleStartPos.x + i * handleInterval;
					cherry->pos.y = handleStartPos.y + j * handleInterval;
					cherry->setSordBaseCenterPos(pos);
					sordCherries.push_back(cherry);
				}
			}
		}
	}

	void SordCherriesManager::barrageUpdate() {
		for (const auto& cherry : sordCherries) {
			cherry->barrageUpdate();
		}
	}

	void SordCherriesManager::setSordBaseCenterPos(Vec2 pos) {
		for (const auto& cherry : sordCherries) {
			cherry->setSordBaseCenterPos(pos);
		}
	}

	void SordCherriesManager::draw() const {
		for (const auto& cherry : sordCherries) {
			cherry->draw();
		}

		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		//TextureAsset(cherryTextureName)(0, 0, textureEdge, textureEdge).scaled(scaleMag).drawAt(pos.x, pos.y - 1, typeColor);
	}
}
