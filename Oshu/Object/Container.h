#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "../Beatmap/Beatmap.h"

namespace Object {

class Container : public sf::Drawable, public sf::Transformable {
public:
	virtual void onMouseClick(uint8_t) {};
	virtual void onMouseDown(uint8_t) {};
	virtual void onMouseUp(uint8_t) {};
	virtual void onMouseMove(uint8_t) {};

	virtual void loadNewSkin() {};

	virtual void update() {};
	
	bool willBeRemove = false;
	bool canClick = true;

	static int renderLayer;

	virtual bool canBeHit(sf::Vector2f pos) {
		return false;
	};
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

	virtual bool canBeHit(sf::Vector2f pos) {
		sf::Vector2f offset = pos - getPosition();
		float dist = sqrt(offset.x * offset.x + offset.y * offset.y);
		return (dist <= (hitObject->CR) / 2);
	}
};

}