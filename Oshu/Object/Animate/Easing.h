#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

// From https://stackoverflow.com/questions/1727881/how-to-use-the-pi-constant-in-c
# define M_PI  3.14159265358979323846  /* pi */
# define M_PIl 3.141592653589793238462643383279502884L /* pi */

namespace Object {
	namespace Animate {
		enum Easing {
			None,
			Out,
			In,
			InQuad,
			OutQuad,
			InOutQuad,
			InCubic,
			OutCubic,
			InOutCubic,
			InQuart,
			OutQuart,
			InOutQuart,
			InQuint,
			OutQuint,
			InOutQuint,
			InSine,
			OutSine,
			InOutSine,
			InExpo,
			OutExpo,
			InOutExpo,
			InCirc,
			OutCirc,
			InOutCirc,
			InElastic,
			OutElastic,
			OutElasticHalf,
			OutElasticQuarter,
			InOutElastic,
			InBack,
			OutBack,
			InOutBack,
			InBounce,
			OutBounce,
			InOutBounce,
			OutPow10,
		};

		double ApplyEasing(Easing easing, double time);

		sf::Vector2f doApply(sf::Vector2f from, sf::Vector2f to, uint32_t atTime, uint32_t duration, Easing easing);

		sf::Color doApply(sf::Color from, sf::Color to, uint32_t atTime, uint32_t duration, Easing easing);

		template <typename T>
		T doApply(T from, T to, uint32_t atTime, uint32_t duration, Easing easing) {
			T out;
			out = to - from;
			out *= ApplyEasing(easing, (double)atTime / (double)duration);
			out += from;
			return out;
		}
	}
}