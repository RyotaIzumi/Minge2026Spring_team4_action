#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Sprite/SpriteSystem.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	class Cherry : public GameObject {
	private:
		//当たり判定サイズ(半径)
		int32 hitBoxSize = 10;

	public:
		double dir = 0;

		bool isDelete = false;//消去用フラグ
		bool isOutOfScreen = false;//画面外判定用フラグ
		bool isTrap = false;//罠用かどうか

		Cherry();

		void update() override;
		virtual void trapUpdate(int32 id);
		void draw() const override;

		void checkOutOfScreen();

		void onCollision(GameObject& other) override;
	};

	class CherryTrap : public Cherry {
	private:
		int32 trapID = 0;
		int32 trapSpeed;
		bool isTrapActived = false;
		bool soundPlayOne = false;
	public:
		CherryTrap(Vec2 startPos, int32 id, double direction, double speed);
		void trapUpdate(int32 id) override;

		int32 getTrapID() const;
	};
}
