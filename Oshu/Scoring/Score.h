#pragma once
#include <map>
#include <string>

#include "enum.h"

namespace Scoring {

class Score {
public:
	ScoreRank::Enum Rank;

	double TotalScore;
	double Accuracy;
	double Health;
	//double PP;
	int MaxCombo;
	int Combo;

	int BeatmapID;

	std::string time;

	std::string User = "";
	std::map<HitResult::Enum, int> Statistics;
};

}