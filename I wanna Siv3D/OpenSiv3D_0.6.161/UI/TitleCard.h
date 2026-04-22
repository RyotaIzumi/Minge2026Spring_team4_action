#pragma once
#include <SIv3D.hpp>
#include "../Global.h"

namespace Iwanna {
	class TitleCard {
	private:
		String titleName;
		Size cardSize{256,195};
		Vec2 startPos, nowPos, cameraPos;
		double moveRangeY;
		Timer moveTimer{ 1.5s,StartImmediately::No };
		Timer waitTimer{ 1.5s,StartImmediately::No };
		int32 moveStep = -1;

		String titleCardText;
		String titleCardTexture;
	public:
		TitleCard();

		void update();
		void setNowCameraPos(Vec2 pos);
		void startShowTitleCard(String roomName);
		void draw() const;
	};
}
