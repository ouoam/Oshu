#pragma once

#include <cmath>

#include "Easing.h"

namespace Object {
	namespace Animate {
		// From https://github.com/ai/easings.net/blob/master/vendor/jquery.easing.js
		// And https://github.com/ppy/osu-framework/blob/master/osu.Framework/MathUtils/Interpolation.cs

		double ApplyEasing(Easing easing, double time) {
			const double elastic_const = 2 * M_PI / .3;
			const double elastic_const2 = .3 / 4;

			const double back_const = 1.70158;
			const double back_const2 = back_const * 1.525;

			const double bounce_const = 1 / 2.75;

			switch (easing) {
			default:
				return time;

			case In:
			case InQuad:
				return time * time;
			case Out:
			case OutQuad:
				return time * (2 - time);
			case InOutQuad:
				if (time < .5) return time * time * 2;
				return --time * time * -2 + 1;

			case InCubic:
				return time * time * time;
			case OutCubic:
				return --time * time * time + 1;
			case InOutCubic:
				if (time < .5) return time * time * time * 4;
				return --time * time * time * 4 + 1;

			case InQuart:
				return time * time * time * time;
			case OutQuart:
				return 1 - --time * time * time * time;
			case InOutQuart:
				if (time < .5) return time * time * time * time * 8;
				return --time * time * time * time * -8 + 1;

			case InQuint:
				return time * time * time * time * time;
			case OutQuint:
				return --time * time * time * time * time + 1;
			case InOutQuint:
				if (time < .5) return time * time * time * time * time * 16;
				return --time * time * time * time * time * 16 + 1;

			case InSine:
				return 1 - std::cos(time * M_PI * .5);
			case OutSine:
				return std::sin(time * M_PI * .5);
			case InOutSine:
				return .5 - .5 * std::cos(M_PI * time);

			case InExpo:
				return std::pow(2, 10 * (time - 1));
			case OutExpo:
				return -std::pow(2, -10 * time) + 1;
			case InOutExpo:
				if (time < .5) return .5 * std::pow(2, 20 * time - 10);
				return 1 - .5 * std::pow(2, -20 * time + 10);

			case InCirc:
				return 1 - std::sqrt(1 - time * time);
			case OutCirc:
				return std::sqrt(1 - --time * time);
			case InOutCirc:
				if ((time *= 2) < 1) return .5 - .5 * std::sqrt(1 - time * time);
				return .5 * std::sqrt(1 - (time -= 2) * time) + .5;

			case InElastic:
				return -std::pow(2, -10 + 10 * time) * std::sin((1 - elastic_const2 - time) * elastic_const);
			case OutElastic:
				return std::pow(2, -10 * time) * std::sin((time - elastic_const2) * elastic_const) + 1;
			case OutElasticHalf:
				return std::pow(2, -10 * time) * std::sin((.5 * time - elastic_const2) * elastic_const) + 1;
			case OutElasticQuarter:
				return std::pow(2, -10 * time) * std::sin((.25 * time - elastic_const2) * elastic_const) + 1;
			case InOutElastic:
				if ((time *= 2) < 1)
					return -.5 * std::pow(2, -10 + 10 * time) * std::sin((1 - elastic_const2 * 1.5 - time) * elastic_const / 1.5);
				return .5 * std::pow(2, -10 * --time) * std::sin((time - elastic_const2 * 1.5) * elastic_const / 1.5) + 1;

			case InBack:
				return time * time * ((back_const + 1) * time - back_const);
			case OutBack:
				return --time * time * ((back_const + 1) * time + back_const) + 1;
			case InOutBack:
				if ((time *= 2) < 1) return .5 * time * time * ((back_const2 + 1) * time - back_const2);
				return .5 * ((time -= 2) * time * ((back_const2 + 1) * time + back_const2) + 2);

			case InBounce:
				time = 1 - time;
				if (time < bounce_const)
					return 1 - 7.5625 * time * time;
				if (time < 2 * bounce_const)
					return 1 - (7.5625 * (time -= 1.5 * bounce_const) * time + .75);
				if (time < 2.5 * bounce_const)
					return 1 - (7.5625 * (time -= 2.25 * bounce_const) * time + .9375);
				return 1 - (7.5625 * (time -= 2.625 * bounce_const) * time + .984375);
			case OutBounce:
				if (time < bounce_const)
					return 7.5625 * time * time;
				if (time < 2 * bounce_const)
					return 7.5625 * (time -= 1.5 * bounce_const) * time + .75;
				if (time < 2.5 * bounce_const)
					return 7.5625 * (time -= 2.25 * bounce_const) * time + .9375;
				return 7.5625 * (time -= 2.625 * bounce_const) * time + .984375;
			case InOutBounce:
				if (time < .5) return .5 - .5 * ApplyEasing(OutBounce, 1 - time * 2);
				return ApplyEasing(OutBounce, (time - .5) * 2) * .5 + .5;

			case OutPow10:
				return --time * std::pow(time, 10) + 1;
			}
		}

		sf::Vector2f doApply(sf::Vector2f from, sf::Vector2f to, uint32_t atTime, uint32_t duration, Easing easing) {
			sf::Vector2f out;
			double change = ApplyEasing(easing, (double)atTime / (double)duration);
			out = to - from;
			out.x *= change;
			out.y *= change;
			out += from;
			return out;
		}

		sf::Color doApply(sf::Color from, sf::Color to, uint32_t atTime, uint32_t duration, Easing easing) {
			sf::Color out;
			double change = ApplyEasing(easing, (double)atTime / (double)duration);
			out = to - from;
			out.r *= change;
			out.g *= change;
			out.b *= change;
			out.a *= change;
			out += from;
			return out;
		}

	}
}