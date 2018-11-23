#pragma once

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
};

class ScoringMode {
public:
	static const enum Enum {
		Standardised,
		Classic
	};
};

}