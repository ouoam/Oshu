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
		circle.scale(sf::Vector2f(0.4, 0.4));
		circle.setScaleFromNow();
	}

	void update() {
		circle.update();
		approach.update();
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(circle, states);
		target.draw(approach, states);
	}
};

}