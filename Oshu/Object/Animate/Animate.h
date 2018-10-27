#pragma once

#include <vector>
#include <queue>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Easing.h"


namespace Object {
	namespace Animate {
		enum doChange {
			Position,
			Scale,
			Origin,
			Color,
			Rotation,
			Opacity,
			Remove
		};

		struct toChange {
			doChange todo;
			uint32_t duration;
			Easing easing;
			union changeTo{
				sf::Vector2f position;
				sf::Vector2f scale;
				sf::Vector2f origin;
				sf::Color color;
				float angle;
				int16_t opacity;

				changeTo() {}
			} to;

			toChange(doChange T, uint32_t D, Easing E ) :
				todo(T), duration(D), easing(E) {}
		};

		class Animate : public sf::Sprite {
		public:
			Animate();
			Animate(const sf::Texture& texture);
			Animate(const sf::Texture& texture, const sf::IntRect& rectangle);

			void update();
			Animate &moveTo			(sf::Vector2f position, uint32_t duration = 0, Easing easing = None);
			Animate &scaleTo		(sf::Vector2f scale,	uint32_t duration = 0, Easing easing = None);
			Animate &moveOriginTo	(sf::Vector2f origin,	uint32_t duration = 0, Easing easing = None);
			Animate &colorTo		(sf::Color color,		uint32_t duration = 0, Easing easing = None);
			Animate &rotateTo		(float angle,			uint32_t duration = 0, Easing easing = None);
			Animate &fadeTo			(uint8_t opacity,		uint32_t duration = 0, Easing easing = None);
			Animate &then();
			Animate &expire();

			bool willBeRemove;
		
		private:
			void init();
			sf::Clock stTime;
			std::queue<std::vector<toChange>> timeline;

			sf::Vector2f stPosition;
			sf::Vector2f stScale;
			sf::Vector2f stOrigin;
			sf::Color stColor;
			float stAngle;
			int16_t stOpacity;

			void addEvent(toChange toAdd);
		};
	}
}