#pragma once

#include <algorithm>
#include <SFML/Graphics.hpp>

#include "Container.h"
#include "Animate/Animate.h"
#include "../Skin/Skin.h"
#include "../Beatmap/Beatmap.h"
#include "../Utility/lineThinkness.h"

#include "Pieces.h"

namespace Object {

class Slider : public ContainerHitObject {
	Animate::AnimeShape sliderBody;

	Circle *circle;

public:
	Slider(Beatmap::bmHitObjects *HitObject) : ContainerHitObject(HitObject) {
		lineThinkness = GenerateTrianglesStrip(hitObject->sliders.curvePoints, hitObject->CR - 5);
		sliderBody.setVertex(lineThinkness);

		sliderBody.fadeTo(0);
		sliderBody.colorTo(sf::Color(128, 0, 0));
		sliderBody.update();

		sliderBody.setOutlineThickness(5);

		circle = new Circle(HitObject);
	}

	void update() {
		sliderBody.update();
		circle->update();
		willBeRemove = sliderBody.willBeRemove && circle->willBeRemove;
	}

	void StartPreemptState() {
		circle->StartPreemptState();

		sliderBody.fadeTo(255, hitObject->TimeFadeIn);
	}

	void onMouseClick(uint8_t key) {
		circle->onMouseClick(key);
		canClick = circle->canClick;

		sliderBody.fadeTo(0, 500).then().expire(); /////////
	}

	void onMiss() {
		circle->miss();

		sliderBody.fadeTo(200).fadeTo(0, 500).then().expire();
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform();
		

		switch (renderLayer) {
		case 1:
			target.draw(sliderBody, states);
			break;
		}
		target.draw(*circle, states);
	}

	sf::VertexArray lineThinkness;
};

}