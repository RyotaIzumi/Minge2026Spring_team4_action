#pragma once
#include <Siv3D.hpp>
#include "../Global.h"

namespace Iwanna {
	inline bool isMoraleBossHpBarEnabled() {
		return Global::moraleValue1 >= 50;
	}

	struct BossHpBarDelayState {
		double yellowRate = 1.0;
		double lastHpRate = 1.0;
		Stopwatch delayStopwatch{ StartImmediately::Yes };
		double delaySeconds = 0.65;
		double catchUpSpeed = 0.55;

		void reset(int32 hp, int32 maxHp) {
			yellowRate = (maxHp > 0) ? Clamp(static_cast<double>(hp) / maxHp, 0.0, 1.0) : 0.0;
			lastHpRate = yellowRate;
			delayStopwatch.restart();
		}

		void update(int32 hp, int32 maxHp) {
			const double hpRate = (maxHp > 0) ? Clamp(static_cast<double>(hp) / maxHp, 0.0, 1.0) : 0.0;
			if (!isMoraleBossHpBarEnabled()) {
				yellowRate = hpRate;
				lastHpRate = hpRate;
				delayStopwatch.restart();
				return;
			}

			if (hpRate < lastHpRate) {
				delayStopwatch.restart();
			}

			if (hpRate < yellowRate) {
				if (delayStopwatch.sF() >= delaySeconds) {
					yellowRate = Max(hpRate, yellowRate - catchUpSpeed * Scene::DeltaTime());
				}
			}
			else {
				yellowRate = hpRate;
				delayStopwatch.restart();
			}
			lastHpRate = hpRate;
		}
	};

	inline void drawBossHpBar(const Vec2& barPos, double width, double height, int32 hp, int32 maxHp, double alpha, const BossHpBarDelayState& delayState) {
		const double hpRate = (maxHp > 0) ? Clamp(static_cast<double>(hp) / maxHp, 0.0, 1.0) : 0.0;

		if (!isMoraleBossHpBarEnabled()) {
			RectF{ barPos.x - width / 2.0, barPos.y, width, height }
				.draw(ColorF{ 1.0, 0.2, 0.2, alpha });
			RectF{ barPos.x - width / 2.0, barPos.y, width * hpRate, height }
				.draw(ColorF{ 0.2, 1.0, 0.2, alpha });
			return;
		}

		RectF{ barPos.x - width / 2.0, barPos.y, width, height }
			.draw(ColorF{ 0.0, 0.0, 0.0, alpha });
		RectF{ barPos.x - width / 2.0, barPos.y, width * delayState.yellowRate, height }
			.draw(ColorF{ 1.0, 0.82, 0.12, alpha });
		RectF{ barPos.x - width / 2.0, barPos.y, width * hpRate, height }
			.draw(ColorF{ 1.0, 0.05, 0.05, alpha });

		const int32 tickCount = Min(maxHp - 1, static_cast<int32>(width / 8.0));
		for (int32 i = 1; i <= tickCount; ++i) {
			const double x = barPos.x - width / 2.0 + width * i / (tickCount + 1);
			const double tickAlpha = (height <= 6.0) ? 0.35 : 0.55;
			Line{ Vec2{ x, barPos.y + 2.0 }, Vec2{ x, barPos.y + height - 2.0 } }
				.draw(1.0, ColorF{ 1.0, 1.0, 1.0, alpha * tickAlpha });
		}
	}
}
