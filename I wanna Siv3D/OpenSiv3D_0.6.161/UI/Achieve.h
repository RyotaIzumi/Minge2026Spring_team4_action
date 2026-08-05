#pragma once
#include <SIv3D.hpp>
#include "../Global.h"

namespace Iwanna {
	enum class AchieveType {
		ItemGet_Heart
	};

	class Achieve {
	private:
		String titleName;
		Size cardSize{ 288,64 };
		Vec2 startPos, nowPos, cameraPos;
		double moveRangeY;
		Timer moveTimer{ 1.2s,StartImmediately::No };
		Timer waitTimer{ 1.5s,StartImmediately::No };
		int32 moveStep = -1;

		bool canShowAchieve = false;

		String achieveText;
		String achieveIconTexture;
		String achieveFrameTexture;
	public:
		Achieve();

		void update();
		void reset();
		void setNowCameraPos(Vec2 pos);
		void startShowAchieve(AchieveType type);
		void draw() const;
	};
}
