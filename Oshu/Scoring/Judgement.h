#pragma once

#include "enum.h"
#include "JudgementResult.h"

namespace Scoring {

class Judgement {
public:
	static int NumericResultFor(HitResult::Enum result) {
		switch (result) {
		default:
			return 0;
		case HitResult::Meh:
			return 50;
		case HitResult::Good:
			return 100;
		case HitResult::Great:
			return 300;
		}
	}

	HitResult::Enum MaxResult = HitResult::Great;

	bool AffectsCombo = true;

	bool IsBonus() {
		return !AffectsCombo;
	}

	int MaxNumericResult() {
		return NumericResultFor(HitResult::Great);
	}

	//int NumericResultFor(JudgementResult result) {
	//	return NumericResultFor(result.Type);
	//}
};

}