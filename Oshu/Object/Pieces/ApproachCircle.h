#pragma once

#include "../Animate/Animate.h"
#include "../../Skin/Skin.h"

namespace Object {

namespace Pieces {

class ApproachCircle : public Animate::AnimeSprite {

public:
	ApproachCircle() {
		sf::Vector2u size = Skin::Hitcircle::approachcircle.getSize();
		setTexture(Skin::Hitcircle::approachcircle);
		setOrigin(size.x / 2.0, size.y / 2.0);
	}
};

}

}