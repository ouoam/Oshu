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

		setScale(scale, scale);

		circle.fadeTo(0);
		circle.colorTo(sf::Color(128, 0, 0));
		circle.update();

		sf::Vector2u size2 = circleOverlay.getTexture()->getSize();
		scale = (float)std::min(size.x, size.y) / (float)std::min(size2.x, size2.y);
		circleOverlay.setScale(scale, scale);
		circleOverlay.setScaleFromNow();
		circleOverlay.fadeTo(0);
		circleOverlay.update();

		size2 = approach.getTexture()->getSize();
		scale = (float)std::min(size.x, size.y) / (float)std::min(size2.x, size2.y);
		approach.setScale(scale, scale);
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
		circle.fadeTo(255, hitObject->TimeFadeIn).moveTo(sf::Vector2f(0, 0));
		circleOverlay.fadeTo(255, hitObject->TimeFadeIn).moveTo(sf::Vector2f(0, 0));
		approach.fadeTo(255, std::min(hitObject->TimeFadeIn * 2, hitObject->TimePreempt));
		approach.scaleTo(1.1, hitObject->TimePreempt);
	}

	void onMouseClick(uint8_t key) {
		approach.fadeTo(255).fadeTo(0, 50)
			.moveTo(sf::Vector2f(0, 0)).then().expire();
		circle.fadeTo(0, 500).scaleTo(1)
			.scaleTo(1.5f, 250, Object::Animate::Easing::OutQuad)
			.moveTo(sf::Vector2f(0, 0)).then().expire();
		circleOverlay.fadeTo(0, 500).scaleTo(1)
			.scaleTo(1.5f, 250, Object::Animate::Easing::OutQuad)
			.then().expire();
		canClick = false;
	}

	void onMiss() {
		circle.fadeTo(200).fadeTo(0, 500)
			.moveTo(sf::Vector2f(0, 0)).then().expire();
		circleOverlay.fadeTo(200).fadeTo(0, 500)
			.moveTo(sf::Vector2f(0, 0)).then().expire();
		approach.fadeTo(200).fadeTo(0, 500)
			.then().expire();
	}

	void shake() {
		circle.fadeTo(255).scaleTo(1)
			.moveTo(sf::Vector2f(-30, 0), 50).then()
			.moveTo(sf::Vector2f(30, 0), 50).then()
			.moveTo(sf::Vector2f(-10, 0), 50).then()
			.moveTo(sf::Vector2f(10, 0), 50).then()
			.moveTo(sf::Vector2f(0, 0), 50);
		circleOverlay.fadeTo(255).scaleTo(1)
			.moveTo(sf::Vector2f(-30, 0), 50).then()
			.moveTo(sf::Vector2f(30, 0), 50).then()
			.moveTo(sf::Vector2f(-10, 0), 50).then()
			.moveTo(sf::Vector2f(10, 0), 50).then()
			.moveTo(sf::Vector2f(0, 0), 50);
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