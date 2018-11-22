#pragma once

#include <algorithm>
#include <SFML/Graphics.hpp>

#include "Container.h"
#include "Animate/Animate.h"
#include "../Skin/Skin.h"
#include "../Beatmap/Beatmap.h"

#include "Pieces.h"

namespace Object {

class Circle : public ContainerHitObject {

	Pieces::CirclePiece circle;
	Pieces::ApproachCircle approach;
	Pieces::CircleOverlay circleOverlay;

public:
	Circle(Beatmap::bmHitObjects *HitObject) : ContainerHitObject(HitObject) {
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
		circle.fadeTo(255, hitObject->TimeFadeIn);
		circleOverlay.fadeTo(255, hitObject->TimeFadeIn);
		approach.fadeTo(255, std::min(hitObject->TimeFadeIn * 2, hitObject->TimePreempt));
		approach.scaleTo(1.1, hitObject->TimePreempt);
		
	}

	void onMouseClick(uint8_t key) {
		approach.fadeTo(255).fadeTo(0, 50).then().expire();
		circle.fadeTo(0, 500).scaleTo(1)
			.scaleTo(1.5f, 250, Object::Animate::Easing::OutQuad).then().expire();
		circleOverlay.fadeTo(0, 500).scaleTo(1)
			.scaleTo(1.5f, 250, Object::Animate::Easing::OutQuad).then().expire();
		canClick = false;
	}

	void onMiss() {
		circle.fadeTo(0, 500).then().expire();
		circleOverlay.fadeTo(0, 500).then().expire();
		approach.fadeTo(0, 500).then().expire();
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform();
		switch (renderLayer) {
		case 0:
			
			break;
		case 1:
			if (canClick) {
				target.draw(circle, states);
				target.draw(circleOverlay, states);
			}
			break;
		case 2:
			if (!canClick) {
				target.draw(circle, states);
				target.draw(circleOverlay, states);
			}
			target.draw(approach, states);
			break;
		}
	}
};

}