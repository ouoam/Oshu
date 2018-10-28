#pragma once

#include <string>

#include <SFML/Graphics.hpp>

namespace Skin {
	std::string base_dir = "resource/default/";

	namespace Cursor {
		sf::Texture cursor;
		sf::Texture cursorMiddle;
		sf::Texture cursorSmoke;
		sf::Texture cursorTrail;

		void load() {
			cursor.loadFromFile(base_dir + "cursor@2x.png");
			cursor.setSmooth(true);
			cursorMiddle.loadFromFile(base_dir + "cursormiddle@2x.png");
			cursorMiddle.setSmooth(true);
			cursorSmoke.loadFromFile(base_dir + "cursor-smoke@2x.png");
			cursorSmoke.setSmooth(true);
			cursorTrail.loadFromFile(base_dir + "cursortrail@2x.png");
			cursorTrail.setSmooth(true);
		}
	}

	namespace Hitburst {
		sf::Texture hit0;
		sf::Texture hit50;
		sf::Texture hit100;
		sf::Texture hit300;

		void load() {
			hit0.loadFromFile(base_dir + "hit0@2x.png");
			hit0.setSmooth(true);
			hit50.loadFromFile(base_dir + "hit50@2x.png");
			hit50.setSmooth(true);
			hit100.loadFromFile(base_dir + "hit100@2x.png");
			hit100.setSmooth(true);
			hit300.loadFromFile(base_dir + "hit300@2x.png");
			hit300.setSmooth(true);
		}
	}

	namespace Hitcircle {
		sf::Texture approachcircle;
		sf::Texture followpoint;
		sf::Texture hitcircle;
		sf::Texture hitcircleoverlay;
		sf::Texture reversearrow;

		void load() {
			approachcircle.loadFromFile(base_dir + "approachcircle@2x.png");
			approachcircle.setSmooth(true);
			followpoint.loadFromFile(base_dir + "followpoint@2x.png");
			followpoint.setSmooth(true);
			hitcircle.loadFromFile(base_dir + "hitcircle@2x.png");
			hitcircle.setSmooth(true);
			hitcircleoverlay.loadFromFile(base_dir + "hitcircleoverlay@2x.png");
			hitcircleoverlay.setSmooth(true);
			reversearrow.loadFromFile(base_dir + "reversearrow@2x.png");
			reversearrow.setSmooth(true);
		}
	}

	namespace Slidertrack {
		sf::Texture sliderb;
		sf::Texture sliderfollowcircle;
		sf::Texture sliderscorepoint;

		void load() {
			sliderb.loadFromFile(base_dir + "sliderb0@2x.png");
			sliderb.setSmooth(true);
			sliderfollowcircle.loadFromFile(base_dir + "sliderfollowcircle@2x.png");
			sliderfollowcircle.setSmooth(true);
			sliderscorepoint.loadFromFile(base_dir + "sliderscorepoint@2x.png");
			sliderscorepoint.setSmooth(true);
		}
	}

	namespace HitcircleNumber{
		sf::Texture default_[10];

		void load() {
			for (int i = 0; i < 10; i++) {
				default_[i].loadFromFile(base_dir + "score-" + (char)(i + '0') + "@2x.png");
				default_[i].setSmooth(true);
			}
			
		}
	}

	void load() {
		Cursor::load();
		Hitburst::load();
		Hitcircle::load();
		Slidertrack::load();
		HitcircleNumber::load();
	}
}