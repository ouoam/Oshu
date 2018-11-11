#pragma once

#include <algorithm>
#include <SFML/Graphics.hpp>

#include "Container.h"
#include "Animate/Animate.h"
#include "../Skin/Skin.h"
#include "../Beatmap/Beatmap.h"

#include "Pieces.h"

namespace Object {

class Circle : public Container {

	Pieces::CirclePiece circle;
	Pieces::ApproachCircle approach;
	Pieces::CircleOverlay circleOverlay;

	Beatmap::bmHitObjects *hitObject;

public:
	Circle(Beatmap::bmHitObjects *HitObject) : hitObject(HitObject) {
		setPosition(sf::Vector2f(hitObject->position));
		sf::Vector2u size = circle.getTexture()->getSize();
		float scale = (float)hitObject->CR / (float)std::min(size.x, size.y);

		circle.scale(sf::Vector2f(scale, scale));
		circle.setScaleFromNow();
		circle.fadeTo(0);
		circle.colorTo(sf::Color(128, 0, 0));
		circle.update();

		circleOverlay.scale(sf::Vector2f(scale, scale));
		circleOverlay.setScaleFromNow();
		circleOverlay.fadeTo(0);
		circleOverlay.update();

		approach.scale(sf::Vector2f(scale, scale));
		approach.setScaleFromNow();
		approach.scaleTo(4);
		approach.fadeTo(0);
		approach.update();
	}

	void update() {
		circle.update();
		circleOverlay.update();
		approach.update();

		willBeRemove = circle.willBeRemove && approach.willBeRemove && circleOverlay.willBeRemove;
	}

	void StartPreemptState() {
		circle.fadeTo(255, hitObject->TimeFadeIn).then();
		circle.fadeTo(255, hitObject->TimePreempt).then();
		circle.fadeTo(0,500).expire();

		circleOverlay.fadeTo(255, hitObject->TimeFadeIn).then();
		circleOverlay.fadeTo(255, hitObject->TimePreempt).then();
		circleOverlay.fadeTo(0, 500).expire();

		approach.fadeTo(255, std::min(hitObject->TimeFadeIn, hitObject->TimePreempt));
		approach.scaleTo(1.1, hitObject->TimePreempt).then();
		approach.fadeTo(255, hitObject->TimePreempt).then();
		approach.fadeTo(0, 500).expire();
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform();
		switch (renderLayer) {
		case 0:
			target.draw(circle, states);
			break;
		case 1:
			target.draw(circleOverlay, states);
			break;
		case 2:
			target.draw(approach, states);
			break;
		}
	}
};

}