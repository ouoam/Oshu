#pragma once

#include "Animate/Animate.h"
#include "../Skin/Skin.h"

namespace Object {

namespace Pieces {

class ApproachCircle : public Object::Animate::AnimeSprite {
public:
	ApproachCircle() : AnimeSprite(Skin::Hitcircle::approachcircle) {}
};

class CirclePiece : public Object::Animate::AnimeSprite {
public:
	CirclePiece() : AnimeSprite(Skin::Hitcircle::hitcircle) {}
};

class CircleOverlay : public Object::Animate::AnimeSprite {
public:
	CircleOverlay() : AnimeSprite(Skin::Hitcircle::hitcircleoverlay) {}
};

}

}