#pragma once

#include "../Animate/Animate.h"
#include "../../Skin/Skin.h"

namespace Object {

namespace Pieces {

class CirclePiece : public Animate::AnimeSprite {

public:
	CirclePiece() {
		sf::Vector2u size = Skin::Hitcircle::hitcircle.getSize();
		setTexture(Skin::Hitcircle::hitcircle);
		setOrigin(size.x / 2.0, size.y / 2.0);
	}
};

}

}