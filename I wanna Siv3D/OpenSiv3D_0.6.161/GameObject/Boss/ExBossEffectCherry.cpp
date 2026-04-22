#include "ExBossCherry.h"
#include "../../Audio/AudioAsset.h"
#include "../../StageManager/BossStageManager.h"

namespace Iwanna {
	// --- 光るりんご --- //
	EffectCherrySpark::EffectCherrySpark(Vec2 startPos, double scale) : Cherry(startPos, scale) {
		pos = startPos;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize * scaleMag);

		canPlayerKill = false;
		isDeleteOutOfScreen = false;
		alpha = 1.0;
		hasAnimation = false;
		cherryTextureName = U"sprSparkCherry";
		depth = 100;

		alpha = 1.0;
		textureEdge = 100;
		scaleMag = 0;

		startAddScale = scale;
		startAddAngle = 12;

		sparkTimer.start();

		setTypeColor();
	}

	void EffectCherrySpark::barrageUpdate() {

		if (sparkTimer.isRunning()) {
			alpha = EaseOutQuart(sparkTimer.progress1_0());
			addAngle = EaseOutQuart(sparkTimer.progress1_0()) * startAddAngle;
			sparkAngle += addAngle;
			addScale = EaseOutQuart(sparkTimer.progress1_0()) * startAddScale;
			scaleMag += addScale;

		}
		else {
			isDelete = true;
		}

		setTypeColor();
	}

	void EffectCherrySpark::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureAsset(cherryTextureName)(0, 0, textureEdge, textureEdge).scaled(scaleMag).rotated(Math::ToRadians(sparkAngle)).drawAt(pos.x, pos.y - 1, ColorF(1.0, alpha));
		//hitBox->draw(Palette::Blue);//判定の可視化
	}
}
