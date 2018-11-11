#pragma once

#include "../Animate/Animate.h"
#include "../../Skin/Skin.h"

#include <iostream>

namespace Object {

namespace Pieces {

class CirclePiece : public Object::Animate::AnimeSprite {

public:
	CirclePiece() : AnimeSprite(Skin::Hitcircle::hitcircle) {}
};

}

}