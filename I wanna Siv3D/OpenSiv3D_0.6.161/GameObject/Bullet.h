#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Sprite/SpriteSystem.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	class Bullet : public GameObject {
	private:
		//当たり判定サイズ(半径)
		int32 hitBoxSize = 2;

		//弾の速度とマップ内最大生成数はStageManagerにて設定

	public:
		bool isOutOfScreen = false;//画面外判定用フラグ

		Bullet(Vec2& pos,double speed);

		void update() override;
		void draw() const override;

		void onCollision(GameObject& other) override;
		void checkOutOfScreen();
	};
}
