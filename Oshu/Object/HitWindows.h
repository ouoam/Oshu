#pragma once

#include <cmath>
#include "../Beatmap/Difficulty.h"
#include "../Scoring/enum.h"

//using namespace Scoring;
//using namespace Beatmap;

namespace Object {

class HitWindows {
protected:
	double Great;
	double Good;
	double Meh;
	double Miss;

public:
	void SetDifficulty(double difficulty) {
		Great = Beatmap::Difficulty::Range(difficulty, 160, 100, 40);
		Good = Beatmap::Difficulty::Range(difficulty, 280, 200, 120);
		Meh = Beatmap::Difficulty::Range(difficulty, 400, 300, 200);
		Miss = Beatmap::Difficulty::Range(difficulty, 400, 400, 400);
	}

	Scoring::HitResult::Enum ResultFor(double timeOffset) {
		timeOffset = std::abs(timeOffset);

		if (timeOffset <= HalfWindowFor(Scoring::HitResult::Great))
			return Scoring::HitResult::Great;
		if (timeOffset <= HalfWindowFor(Scoring::HitResult::Good))
			return Scoring::HitResult::Good;
		if (timeOffset <= HalfWindowFor(Scoring::HitResult::Meh))
			return Scoring::HitResult::Meh;
		if (timeOffset <= HalfWindowFor(Scoring::HitResult::Miss))
			return Scoring::HitResult::Miss;

		return Scoring::HitResult::None;
	}

	double HalfWindowFor(Scoring::HitResult::Enum result) {
		switch (result) {
		case Scoring::HitResult::Great:
			return Great / 2;
		case Scoring::HitResult::Good:
			return Good / 2;
		case Scoring::HitResult::Meh:
			return Meh / 2;
		case Scoring::HitResult::Miss:
			return Miss / 2;
		}
	}

	bool CanBeHit(double timeOffset) {
		return timeOffset <= HalfWindowFor(Scoring::HitResult::Meh);
	}
};

}