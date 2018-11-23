#pragma once

#include <map>
#include <algorithm>

#include "../Beatmap/Beatmap.h"
#include "../Utility/stl.h"

#include "enum.h"
#include "Score.h"
//#include "Judgement.h"
#include "JudgementResult.h"

namespace Scoring {

class ScoreProcessor {
private:
	float hpDrainRate = 0;
	const double harshness = 0.01;

	const double base_portion = 0.3;
	const double combo_portion = 0.7;
	const double max_score = 1000000;

	double maxHighestCombo = 0;

	double maxBaseScore = 0;
	double rollingMaxBaseScore = 0;
	double baseScore = 0;
	double bonusScore = 0;

	std::map<HitResult::Enum, int> scoreResultCounts;
	std::map<ComboResult::Enum, int> comboResultCounts;

	void updateScore() {
		if (rollingMaxBaseScore != 0)
			Accuracy = baseScore / rollingMaxBaseScore;

		Rank = rankFrom(Accuracy);

		switch (Mode) {
		case ScoringMode::Standardised:
			TotalScore= max_score * (base_portion * baseScore / maxBaseScore + combo_portion * HighestCombo / maxHighestCombo) + bonusScore;
			break;
		case ScoringMode::Classic:
			// should emulate osu-stable's scoring as closely as we can (https://osu.ppy.sh/help/wiki/Score/ScoreV1)
			TotalScore = bonusScore + baseScore * (1 + std::max(0, HighestCombo - 1) / 25.0);
			break;
		}
	}



	void applyResult(JudgementResult result) {
		result.ComboAtJudgement = Combo;
		result.HighestComboAtJudgement = HighestCombo;

		JudgedHits++;

		if (result.Judgement.AffectsCombo) {
			switch (result.Type) {
			case HitResult::None:
				break;
			case HitResult::Miss:
				Combo = 0;
				break;
			default:
				Combo++;
				break;
			}
		}

		HighestCombo = std::max(Combo, HighestCombo);

		if (result.Judgement.IsBonus()) {
			if (result.IsHit())
				bonusScore += result.Judgement.NumericResultFor(result.Type);
		}
		else {
			baseScore += result.Judgement.NumericResultFor(result.Type);
			rollingMaxBaseScore += result.Judgement.MaxNumericResult();
		}

		if (result.Type != HitResult::None) {
			scoreResultCounts[result.Type] = GetWithDef(scoreResultCounts, result.Type, 0) + 1;
			comboResultCounts[result.ComboType] = GetWithDef(comboResultCounts, result.ComboType, 0) + 1;
		}

		switch (result.Type) {
		case HitResult::Great:
			Health += (10.2 - hpDrainRate) * harshness;
			break;

		case HitResult::Good:
			Health += (8 - hpDrainRate) * harshness;
			break;

		case HitResult::Meh:
			Health += (4 - hpDrainRate) * harshness;
			break;

		case HitResult::Miss:
			Health -= hpDrainRate * (harshness * 2);
			break;
		}
	}

	void Reset(bool storeResults) {
		if (storeResults) {
			MaxHits = JudgedHits;
			maxHighestCombo = HighestCombo;
			maxBaseScore = baseScore;
		}

		TotalScore = 0;
		Accuracy = 1;
		Health = 1;
		Combo = 0;
		Rank = ScoreRank::X;
		HighestCombo = 0;

		HasFailed = false;

		JudgedHits = 0;
		baseScore = 0;
		rollingMaxBaseScore = 0;
		bonusScore = 0;

		scoreResultCounts.clear();
		comboResultCounts.clear();
	}

	ScoreRank::Enum rankFrom(double acc) {
		if (acc == 1)
			return ScoreRank::X;
		if (acc > 0.95)
			return ScoreRank::S;
		if (acc > 0.9)
			return ScoreRank::A;
		if (acc > 0.8)
			return ScoreRank::B;
		if (acc > 0.7)
			return ScoreRank::C;
		return ScoreRank::D;
	}

	void UpdateFailed() {
		if (HasFailed)
			return;

		if (!(Health == 0))
			return;

		HasFailed = true;
	}

	void SimulateAutoplay(Beatmap::Beatmap *beatmap) {
		auto simulate = [&](Beatmap::bmHitObjects obj) {
			//foreach(var nested in obj.NestedHitObjects)
			//	simulate(nested);

			Judgement judgement;

			auto result = CreateResult(judgement);

			result.Type = judgement.MaxResult;

			applyResult(result);
		};

		for (auto obj : beatmap->HitObjects)
			simulate(obj);
	}

	int MaxHits = 0;
	int JudgedHits = 0;

	void ApplyBeatmap(Beatmap::Beatmap *beatmap) {
		hpDrainRate = beatmap->Difficulty.HPDrainRate;
	}

	

public:
	

	JudgementResult CreateResult(Judgement judgement) {
		return JudgementResult(judgement);
	}

	void ApplyResult(JudgementResult result) {
		applyResult(result);
		updateScore();

		UpdateFailed();
	}

	ScoreRank::Enum Rank;
	ScoringMode::Enum Mode = ScoringMode::Standardised;
	double TotalScore = 0;
	double Accuracy = 0;
	double Health = 1;
	int HighestCombo = 0;
	int Combo = 0;

	bool hasCompleted() {
		return JudgedHits + 1 >= MaxHits;  // ?????????
	};
	bool HasFailed = false;

	void PopulateScore(Score *score) {
		score->TotalScore = TotalScore;
		score->Combo = Combo;
		score->MaxCombo = HighestCombo;
		score->Accuracy = Accuracy;
		score->Rank = Rank;
		//score->Date = DateTimeOffset.Now;
		score->Health = Health;

		score->Statistics[HitResult::Great] = GetWithDef(scoreResultCounts, HitResult::Great, 0);
		score->Statistics[HitResult::Good]  = GetWithDef(scoreResultCounts, HitResult::Good, 0);
		score->Statistics[HitResult::Meh]   = GetWithDef(scoreResultCounts, HitResult::Meh, 0);
		score->Statistics[HitResult::Miss]  = GetWithDef(scoreResultCounts, HitResult::Miss, 0);
	}

	//ScoreProcessor(RulesetContainer<TObject> rulesetContainer) {
	ScoreProcessor(Beatmap::Beatmap *beatmap) {
		//rulesetContainer.OnNewResult += applyResult;
		//rulesetContainer.OnRevertResult += revertResult;

		ApplyBeatmap(beatmap);
		SimulateAutoplay(beatmap);
		Reset(true);

		if (maxBaseScore == 0 || maxHighestCombo == 0) {
			Mode = ScoringMode::Classic;
		}
	}
};

}