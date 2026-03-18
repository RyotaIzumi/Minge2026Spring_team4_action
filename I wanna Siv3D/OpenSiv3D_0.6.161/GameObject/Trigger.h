#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	class Trigger : public GameObject {
	private:
		int32 side = 32;
		int32 trapID = 0;
		bool isActivated = false;//トリガーが作動しているかどうか
		bool checkPrevID = false;//ひとつ前のトリガーIDが起動している場合のみ自身を起動させる
		bool checkOtherCondition = false;//トリガー起動に他条件を必要とするかどうか
	public:
		Trigger(Vec2 startPos, int32 id, double sizeX, double sizeY, bool checkPrevId);
		Trigger(Vec2 startPos, int32 id, double sizeX, double sizeY, std::function<bool()> checkOtherConditionFunc);

		void update() override;
		void draw() const override;
		void onCollision(GameObject& other) override;

		std::function<bool()> checkOtherConditionFunc;

		// トリガーを作動させる
		void triggerActivate();
		bool getIsActivated() const;
		bool getCheckPrevID() const;
		bool getCheckOtherCondition() const;
		int32 getTrapID() const;
	};
}
