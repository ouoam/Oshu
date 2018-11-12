#pragma once

#include <SFML/Graphics.hpp>
#include "../Beatmap/Beatmap.h"

namespace Object {

class Container : public sf::Drawable, public sf::Transformable {
public:
	virtual void onMouseClick(uint8_t) {};
	virtual void onMouseDown(uint8_t) {};
	virtual void onMouseUp(uint8_t) {};
	virtual void onMouseMove(uint8_t) {};

	virtual void update() {};
	virtual void StartPreemptState() {};

	bool willBeRemove = false;

	static int renderLayer;

	Beatmap::bmHitObjects *hitObject;

	Container(Beatmap::bmHitObjects *HitObject) : hitObject(HitObject) {}
};

}