#pragma once
#include <SIv3D.hpp>
#include "../Global.h"

namespace Iwanna {
	class TitleCard {
	private:
		String titleName;
		Size cardSize{256,195};
		Vec2 startPos, showPos, nowPos, cameraPos;
		double moveRangeY;
		Timer moveTimer{ 1.2s,StartImmediately::No };
		Timer waitTimer{ 1.5s,StartImmediately::No };
		int32 moveStep = -1;

		bool canShowTitleCard = false;

		String titleCardText;
		String titleCardTexture;
	public:
		TitleCard();

		void update();
		void reset();
		void setNowCameraPos(Vec2 pos);
		void setEntryByPlayerPosition(Vec2 playerPos, Vec2 viewCenter);
		void startShowTitleCard(String roomName);
		void draw() const;
	};
}
