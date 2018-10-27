#include "Animate.h"

namespace Object {
	namespace Animate {
		void Animate::init() {
			willBeRemove = false;

			stPosition = getPosition();
			stScale = getScale();
			stOrigin = getOrigin();
			stColor = getColor();
			stAngle = getRotation();
			stOpacity = getColor().a;
		}

		Animate::Animate()
		{
			init();
		}

		Animate::Animate(const sf::Texture& texture)
		{
			init();
			setTexture(texture);
			
		}

		Animate::Animate(const sf::Texture& texture, const sf::IntRect& rectangle)
		{
			init();
			setTexture(texture);
			setTextureRect(rectangle);
		}

		void Animate::update() {
			if (!timeline.empty()) {
				uint32_t elapsed = stTime.getElapsedTime().asMilliseconds();

				std::vector<toChange> *front;

				front = &timeline.front();

				if (!(*front).empty()) {
					std::vector<toChange>::iterator it = (*front).begin();
					while (it != (*front).end()) {
						if (it->duration <= elapsed) {
							sf::Color tempColor = getColor();

							switch (it->todo) {
							case Position:
								setPosition(it->to.position);
								stPosition = it->to.position;
								break;
							case Scale:
								setScale(it->to.scale);
								stScale = it->to.scale;
								break;
							case Origin:
								setOrigin(it->to.origin);
								stOrigin = it->to.origin;
								break;
							case Color:
								setColor(it->to.color);
								stColor = it->to.color;
								break;
							case Rotation:
								setRotation(it->to.angle);
								stAngle = it->to.angle;
								break;
							case Opacity:
								tempColor.a = it->to.opacity;
								setColor(tempColor);
								stOpacity = it->to.opacity;
								break;
							case Remove:
								willBeRemove = true;
								break;
							}

							// Thank https://stackoverflow.com/questions/15042942/vector-iterators-incompatible-while-erase-from-vector
							it = (*front).erase(it);

							if (it != (*front).begin())
							{
								it = std::prev(it);
								continue;
							}
							
						} else {
							sf::Color tempColor = getColor();
							switch (it->todo) {
							case Position:
								setPosition(doApply(stPosition, it->to.position, elapsed, it->duration, it->easing));
								break;
							case Scale:
								setScale(doApply(stScale, it->to.scale, elapsed, it->duration, it->easing));
								break;
							case Origin:
								setOrigin(doApply(stOrigin, it->to.origin, elapsed, it->duration, it->easing));
								break;
							case Color:
								setColor(doApply(stColor, it->to.color, elapsed, it->duration, it->easing));
								break;
							case Rotation:
								setRotation(doApply(stAngle, it->to.angle, elapsed, it->duration, it->easing));
								break;
							case Opacity:
								tempColor.a = doApply(stOpacity, it->to.opacity, elapsed, it->duration, it->easing);
								setColor(tempColor);
								break;
							case Remove:
								willBeRemove = true;
								break;
							}

							++it;
						}
					}
				}

				if (timeline.front().empty()) {
					stTime.restart();
					timeline.pop();
				}
			}
		}

		void Animate::addEvent(toChange toAdd) {
			if (timeline.empty()) {
				std::vector<toChange> temp;
				timeline.push(temp);		// For get start value
				timeline.push(temp);		// For push data to
			}

			timeline.back().push_back(toAdd);
		}

		Animate &Animate::moveTo(sf::Vector2f position, uint32_t duration, Easing easing) {
			toChange toPush(Position, duration, easing);
			toPush.to.position = position;

			addEvent(toPush);
			return *this;
		}

		Animate &Animate::scaleTo(sf::Vector2f scale, uint32_t duration, Easing easing) {
			toChange toPush(Scale, duration, easing);
			toPush.to.scale = scale;

			addEvent(toPush);
			return *this;
		}

		Animate &Animate::moveOriginTo(sf::Vector2f origin, uint32_t duration, Easing easing) {
			toChange toPush(Origin, duration, easing);
			toPush.to.origin = origin;

			addEvent(toPush);
			return *this;
		}

		Animate &Animate::colorTo(sf::Color color, uint32_t duration, Easing easing) {
			toChange toPush(Color, duration, easing);
			toPush.to.color = color;

			addEvent(toPush);
			return *this;
		}

		Animate &Animate::rotateTo(float angle, uint32_t duration, Easing easing) {
			toChange toPush(Rotation, duration, easing);
			toPush.to.angle = angle;

			addEvent(toPush);
			return *this;
		}

		Animate &Animate::fadeTo(uint8_t opacity, uint32_t duration, Easing easing) {
			toChange toPush(Opacity, duration, easing);
			toPush.to.opacity = opacity;

			addEvent(toPush);
			return *this;
		}

		Animate &Animate::then() {
			std::vector<toChange> temp;
			timeline.push(temp);

			return *this;
		}
		Animate &Animate::expire() {
			toChange toPush(Remove, 0, None);

			addEvent(toPush);
			return *this;
		}

	}
}
