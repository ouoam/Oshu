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
	
	bool willBeRemove = false;
	bool canClick = true;

	static int renderLayer;
};

class ContainerHitObject : public Container {
public:
	Beatmap::bmHitObjects *hitObject;

	bool isMiss = false;

	void miss() {
		if(!isMiss)
			onMiss();
		isMiss = true;
	}

	virtual void StartPreemptState() {};

	virtual void shake() { };

	virtual void onMiss() { };

	ContainerHitObject(Beatmap::bmHitObjects *HitObject) : hitObject(HitObject) {}
};

}