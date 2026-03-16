#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	class SpecialTrap : public GameObject {
	protected:
		int32 gridSide = 32;
		int32 trapID = 0;
		int32 nowTrapID = 0;//game側で起動中のid
		int32 trapStep = 0;
		bool isActivated = false;//トリガーが作動しているかどうか
		Stopwatch trapStopwatch;

		//テクスチャ関連
		String textureName;
		Vec2 textureSize;
		double textureScale = 1.0;
		double textureAlpha = 1.0;
	public:
		SpecialTrap(Vec2 startPos, int32 id);

		void update() override;
		virtual void trapUpdate();
		void draw() const override;
		void onCollision(GameObject& other) override;

		void specialTrapActivate();
		bool getIsActivated() const;
		int32 getTrapID() const;
		void setNowTrapID(int32 id);

		bool reachedTrapTime(double time);
	};

	class WarningWindowTrap : public SpecialTrap {
	private:
		Timer scaleTimer{0.15s};
	public:
		WarningWindowTrap(Vec2 startPos, int32 id);
		void trapUpdate() override;
		void draw() const override;
	};

	class SteamTrap : public SpecialTrap {
	private:
		Timer moveTimer{ 0.7s };
		double moveRange = 96;
		Vec2 basePos;
	public:
		SteamTrap(Vec2 startPos, int32 id);
		void trapUpdate() override;
		void draw() const override;
	};

	class TreeTrap : public SpecialTrap {
	private:
		Timer moveTimer{ 2s };
		double moveRange = 1000;
		Vec2 basePos;
		Vec2 hitboxPos;
	public:
		TreeTrap(Vec2 startPos, int32 id,String name);
		void trapUpdate() override;
		void draw() const override;
	};

	class TitleTrap : public SpecialTrap {
	private:
		Timer moveTimer{ 3s };
		double moveRange = 1500;
		Vec2 basePos;
	public:
		TitleTrap(Vec2 startPos, int32 id);
		void trapUpdate() override;
		void draw() const override;
	};

	class TitleTrap2 : public SpecialTrap {
	private:
		Timer moveTimer{ 7.5s };
		double moveRange = 1500;
		Vec2 basePos;
	public:
		TitleTrap2(Vec2 startPos, int32 id);
		void trapUpdate() override;
		void draw() const override;
	};

	class AdWindowTrap : public SpecialTrap {
	private:
		Timer scaleTimer{ 0.15s };
		Vec2 deleteButtonPos;
		bool isShowHideLine = false;
		String holeText;
	public:
		AdWindowTrap(Vec2 startPos, int32 id);
		void trapUpdate() override;
		void draw() const override;
	};
}
