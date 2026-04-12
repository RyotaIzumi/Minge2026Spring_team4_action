#pragma once
#include <Siv3D.hpp>

namespace Iwanna {
	//カメラの揺れを管理する構造体
	struct CameraShake
	{
		double time = 0.0;
		double power = 0.0;
		double frequency = 30;

		void shake(double t = 0.5, double p = 10.0)
		{
			time = t;
			power = p;
		}

		void update()
		{
			if (time > 0.0)	time -= Scene::DeltaTime();
		}

		Vec2 getOffset() const
		{
			if (time <= 0.0)
			{
				return Vec2{ 0, 0 };
			}
			double t = Scene::Time();
			// 縦方向のみ
			double y = Math::Sin(t * frequency) * power * time;
			return Vec2{ 0, y };
		}
	};
}
