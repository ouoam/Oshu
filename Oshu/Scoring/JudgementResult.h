#pragma once

#include "enum.h"
#include "Judgement.h"

namespace Scoring {

class JudgementResult {
public:
	HitResult::Enum Type = HitResult::None;
	ComboResult::Enum ComboType = ComboResult::None;

	Judgement Judgement;
	int TimeOffset;
	int ComboAtJudgement;
	int HighestComboAtJudgement;

	bool HasResult() {
		return Type > HitResult::None;
	}
	bool IsHit() {
		return Type > HitResult::Miss;
	}

	JudgementResult(Scoring::Judgement judgement) {
		Judgement = judgement;
	}

	JudgementResult(HitResult::Enum hitResult) {
		Type = hitResult;
	}
};

}