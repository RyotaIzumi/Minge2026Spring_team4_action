#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Sprite/SpriteSystem.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	enum class SaveType {
		Normal,
		MoveTrap,
		FakeTrap
	};

	class SavePoint : public GameObject {
	protected:
		int32 side = 32;
		bool isSaving = false;
		Timer saveIntervalTimer{ SecondsF{ 1.0 }, StartImmediately::No };

		//判定調整用
		Vec2 hitBoxSize = {32,24};
		double gapY = 8;
		//罠関連
		bool isTrap = false;
		int32 trapID = 0;
	public:
		SavePoint(Vec2 startPos);

		SaveType saveType;

		void update() override;
		virtual void trapUpdate(int32 id);
		void draw() const override;
		void saved();
		std::function<void()> onSavedCallback;
		void onCollision(GameObject& other) override;
		bool getIsTrap() const;
		int32 getTrapID() const;
	};

	class SaveMoveTrap : public SavePoint {
	private:
		bool isStartTrap;
	public:
		SaveMoveTrap(Vec2 startPos, int32 id, double spd, double dir);

		void trapUpdate(int32 id) override;
		bool getIsStartTrap() const;
	};

	class SaveFakeTrap : public SavePoint {
	private:
		bool isStartTrap;
	public:
		SaveFakeTrap(Vec2 startPos);

		void trapUpdate(int32 id) override;
		void setIsStartTrap(bool);
		bool getIsStartTrap() const;
		void draw() const override;
	};
}
