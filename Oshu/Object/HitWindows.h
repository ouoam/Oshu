#pragma once

#include <cmath>
#include "../Beatmap/Difficulty.h"
#include "../Scoring/HitResult.h"

using namespace Scoring;
using namespace Beatmap;

namespace Object {

class HitWindows {
protected:
	double Great;
	double Good;
	double Meh;
	double Miss;

public:
	void SetDifficulty(double difficulty) {
		Great = Difficulty::Range(difficulty, 160, 100, 40);
		Good = Difficulty::Range(difficulty, 280, 200, 120);
		Meh = Difficulty::Range(difficulty, 400, 300, 200);
		Miss = Difficulty::Range(difficulty, 400, 400, 400);
	}

	HitResult ResultFor(double timeOffset) {
		timeOffset = std::abs(timeOffset);

		if (timeOffset <= HalfWindowFor(HitResult::Great))
			return HitResult::Great;
		if (timeOffset <= HalfWindowFor(HitResult::Good))
			return HitResult::Good;
		if (timeOffset <= HalfWindowFor(HitResult::Meh))
			return HitResult::Meh;
		if (timeOffset <= HalfWindowFor(HitResult::Miss))
			return HitResult::Miss;

		return HitResult::None;
	}

	double HalfWindowFor(HitResult result) {
		switch (result) {
		case HitResult::Great:
			return Great / 2;
		case HitResult::Good:
			return Good / 2;
		case HitResult::Meh:
			return Meh / 2;
		case HitResult::Miss:
			return Miss / 2;
		}
	}

	bool CanBeHit(double timeOffset) {
		return timeOffset <= HalfWindowFor(HitResult::Meh);
	}
};

}