#pragma once

#include "Animate/Animate.h"
#include "../Skin/Skin.h"

namespace Object {

namespace Pieces {

class HitPieces : public Object::Animate::AnimeSprite {
public:
	HitPieces(const sf::Texture& texture) : AnimeSprite(texture) {
		sf::Vector2u size = texture.getSize();
		setOrigin(size.x / 2, size.y / 2);
	}
};

class ApproachCircle : public HitPieces {
public:
	ApproachCircle() : HitPieces(Skin::Hitcircle::approachcircle) {}
};

class CirclePiece : public HitPieces {
public:
	CirclePiece() : HitPieces(Skin::Hitcircle::hitcircle) {}
};

class CircleOverlay : public HitPieces {
public:
	CircleOverlay() : HitPieces(Skin::Hitcircle::hitcircleoverlay) {}
};

}

}