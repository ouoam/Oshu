#pragma once

#include <SFML/Graphics.hpp>

#include "Container.h"
#include "Animate/Animate.h"
#include "../Skin/Skin.h"

#include "Pieces/ApproachCircle.h"
#include "Pieces/CirclePiece.h"

namespace Object {

class Circle : public Container {

	Pieces::CirclePiece circle;
	Pieces::ApproachCircle approach;

public:
	Circle() {
		circle.scale(sf::Vector2f(0.2, 0.2));
		circle.setScaleFromNow();
		circle.fadeTo(0);
		circle.update();
		approach.scale(sf::Vector2f(0.2, 0.2));
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
		circle.fadeTo(255, 400);
		approach.fadeTo(255, 600);
		approach.scaleTo(1.1, 600);
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