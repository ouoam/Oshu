#pragma once

#include "../Animate/Animate.h"
#include "../../Skin/Skin.h"

namespace Object {

namespace Pieces {

class ApproachCircle : public Animate::AnimeSprite {

public:
	ApproachCircle() : AnimeSprite(Skin::Hitcircle::approachcircle) {}
};

}

}