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

class Slider : public Container {
	Beatmap::bmHitObjects *hitObject;
	Animate::AnimeShape sliderBody;

	Circle *circle;

public:
	Slider(Beatmap::bmHitObjects *HitObject) : hitObject(HitObject) {
		lineThinkness = GenerateTrianglesStrip(hitObject->sliders.curvePoints, hitObject->CR);
		sliderBody.setVertex(lineThinkness);

		sliderBody.fadeTo(0);
		sliderBody.colorTo(sf::Color(255, 0, 0));
		sliderBody.update();

		sliderBody.setOutlineThickness(-5);

		circle = new Circle(HitObject);
		
	}

	void update() {
		sliderBody.update();
		circle->update();
		willBeRemove = sliderBody.willBeRemove && circle->willBeRemove;
	}

	void StartPreemptState() {
		sliderBody.fadeTo(255, hitObject->TimeFadeIn).then();
		sliderBody.fadeTo(255, 0).then().expire();

		circle->StartPreemptState();
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform();
		target.draw(*circle, states);

		switch (renderLayer) {
		case 0:
			target.draw(sliderBody, states);
			break;
		}
	}

	sf::VertexArray lineThinkness;
};

}