#pragma once

#include <SFML/Graphics.hpp>

#include "Container.h"
#include "Animate/Animate.h"
#include "../Skin/Skin.h"

#include "Pieces.h"

namespace Object {

class Circle : public Container {

	Pieces::CirclePiece circle;
	Pieces::ApproachCircle approach;

public:
	Circle() {
		circle.scale(sf::Vector2f(0.4, 0.4));
		circle.setScaleFromNow();
		circle.fadeTo(0);
		circle.update();
		approach.scale(sf::Vector2f(0.4, 0.4));
		approach.setScaleFromNow();
		approach.scaleTo(4);
		approach.fadeTo(0);
		approach.update();
	}

	void update() {
		circle.update();
		approach.update();

		willBeRemove = circle.willBeRemove && approach.willBeRemove;
	}

	void StartPreemptState() {
		circle.fadeTo(255, 400).then();
		//circle.fadeTo(255, 2000).then().expire();
		approach.fadeTo(255, 600);
		approach.scaleTo(1.1, 600).then();
		//approach.fadeTo(255, 2000).then().expire();
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform();
		target.draw(circle, states);
		target.draw(approach, states);
		//target.draw(circle);
		//target.draw(approach);
	}
};

}