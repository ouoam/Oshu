#pragma once

namespace Beatmap {

class Difficulty {
public:
	static double Range(double difficulty, double min, double mid, double max) {
		if (difficulty > 5)
			return mid + (max - mid) * (difficulty - 5.0) / 5.0;
		if (difficulty < 5)
			return mid - (mid - min) * (5.0 - difficulty) / 5.0;
		return mid;
	}
};

}