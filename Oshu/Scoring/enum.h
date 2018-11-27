#pragma once

#include <string>

namespace Scoring {

class HitResult {
public:
	static const enum Enum {
		None,
		Miss,
		Meh,
		Good,
		Great
	};
};


class ComboResult {
public:
	static const enum Enum {
		None,
		Good,
		Perfect
	};
};

class ScoreRank {
public:
	static const enum Enum {
		F,
		D,
		C,
		B,
		A,
		S,
		SH,
		X,
		XH
	};

	static std::string tostring(Enum e) {
		switch (e) {
		case F: return "F";
		case D: return "D";
		case C: return "C";
		case B: return "B";
		case A: return "A";
		case S: return "S";
		case SH: return "SH";
		case X: return "X";
		case XH: return "XH";
		}
	}
};

class ScoringMode {
public:
	static const enum Enum {
		Standardised,
		Classic
	};
};

}