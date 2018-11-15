#pragma once

#include <iostream>
#include <deque>
#include <cmath>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "../Beatmap/Beatmap.h"
#include "../Audio/hitsound.h"
#include "../Object/Container.h"
#include "../Object/Cursor.h"
#include "../Object/Circle.h"
#include "../Object/Slider.h"

#include "UI.h"

class testUI : public UI {

	Object::Cursor cur;

	std::deque<Object::Container*> objs;

	sf::Mutex Mutex;

	sf::Transform transform;

	float mspb = 0;
	float oldmspb = 0;

	int lastTimePass = 0;
	int nextHit = -1;
	int lastHit = -1;
	int hit = 0;
	int combo = 0;
	int lastDelay = 0;
	int dist = 0;

	struct {
		int s50, s100, s300;
	} hitWinWidth;

	struct {
		int front = 0;
		int back = 0;
	} showHitObj;

	sf::Music music;
	Beatmap::Beatmap bmPlay;

protected:


	void OnPressed(sf::Event event) {
		cur.onMouseDown(event.key.code);

		Mutex.lock();

		sf::Vector2f click = sf::Vector2f(sf::Mouse::getPosition(m_window));

		std::deque<Object::Container*>::iterator it = objs.begin();
		std::deque<Object::Container*>::iterator end = objs.end();
		while (it != end) {
			if ((*it)->canClick) {
				sf::Vector2f offset = click - transform.transformPoint(sf::Vector2f((*it)->hitObject->position));

				std::cout << offset.x << "\t" << offset.y << std::endl;

				float dist = sqrt(offset.x * offset.x + offset.y * offset.y);

				if (dist <= ((*it)->hitObject->CR) / 2) {
					std::cout << (int)dist << "hit\n";
					(*it)->onMouseClick(event.key.code);
					break;
				}
			}
			++it;
		}
		Mutex.unlock();
	}

	void OnReleased(sf::Event event) {
		cur.onMouseUp(event.key.code);
	}

public:

	testUI(sf::RenderWindow& window) : UI(window) , cur(window){

		//std::string base_dir = "resource/150945 Knife Party - Centipede/";
		//bmPlay.load(base_dir + "Knife Party - Centipede (Sugoi-_-Desu) [This isn't a map, just a simple visualisation].osu");
	

		std::string base_dir = "resource/499488 Kana Nishino - Sweet Dreams (11t dnb mix)/";
		bmPlay.load(base_dir + "Kana Nishino - Sweet Dreams (11t dnb mix) (Ascendance) [ReFaller's Hard].osu");

		loadHitSound(&bmPlay, base_dir);

		hitWinWidth.s50 = 150 - 50 * (5 - bmPlay.Difficulty.OverallDifficulty) / 5;
		hitWinWidth.s100 = 100 - 40 * (5 - bmPlay.Difficulty.OverallDifficulty) / 5;
		hitWinWidth.s300 = 50 - 30 * (5 - bmPlay.Difficulty.OverallDifficulty) / 5;

		// Calc For Hit Object
		int AR = bmPlay.Difficulty.ApproachRate;

		if (AR < 5) {
			Beatmap::bmHitObjects::TimePreempt = 1200 + 600 * (5 - AR) / 5;
			Beatmap::bmHitObjects::TimeFadeIn = 800 + 400 * (5 - AR) / 5;
		}
		else if (AR > 5) {
			Beatmap::bmHitObjects::TimePreempt = 1200 - 750 * (AR - 5) / 5;
			Beatmap::bmHitObjects::TimeFadeIn = 800 - 500 * (AR - 5) / 5;
		}

		Beatmap::bmHitObjects::CR = bmPlay.Difficulty.CircleRadius;
		// End Calc For Hit Object

		transform.translate(80, 60);

		if (!music.openFromFile("resource/audio.wav"))
			return; // error
		music.setVolume(50);
		music.play();


		//music.setPlayingOffset(sf::seconds(40));
	}


	void update() {
		int64_t time = music.getPlayingOffset().asMilliseconds();

		// set volume of sound effect
		if (bmPlay.iTimingPoints < bmPlay.TimingPoints.size()) {
			if (bmPlay.TimingPoints[bmPlay.iTimingPoints].Offset <= time) {
				for (int i = 0; i < 10; i++) {
					sound[i].setVolume(bmPlay.TimingPoints[bmPlay.iTimingPoints].Volume);
				}
				if (bmPlay.TimingPoints[bmPlay.iTimingPoints].mspb > 0) {
					mspb = bmPlay.TimingPoints[bmPlay.iTimingPoints].mspb;
					oldmspb = mspb;
				}
				else {
					mspb = oldmspb * -(bmPlay.TimingPoints[bmPlay.iTimingPoints].mspb) / 100;
				}

				bmPlay.iTimingPoints++;
			}
		}

		int HOsize = bmPlay.HitObjects.size();

		// set new hit obj to show
		while (showHitObj.back < HOsize) {
			if (bmPlay.HitObjects[showHitObj.back].time - Beatmap::bmHitObjects::TimePreempt <= time) {
				showHitObj.back++;

				if (bmPlay.HitObjects[showHitObj.front].type & 1) { // circle
					Object::Circle *newCircle = new Object::Circle(&bmPlay.HitObjects[showHitObj.front]);
					newCircle->StartPreemptState();

					Mutex.lock();
					objs.push_back(newCircle);
					Mutex.unlock();
				} else if (bmPlay.HitObjects[showHitObj.front].type & 2) { // slider
					Object::Slider *newSlider = new Object::Slider(&bmPlay.HitObjects[showHitObj.front]);
					newSlider->StartPreemptState();

					Mutex.lock();
					objs.push_back(newSlider);
					Mutex.unlock();
				}
			}
			else break;
		}
	
		while (showHitObj.front < HOsize ) {
			int endTime = 0;
			if (bmPlay.HitObjects[showHitObj.front].type & 1) { // circle
				endTime = bmPlay.HitObjects[showHitObj.front].time + hitWinWidth.s50; /////////////////////////// 
			}
			else if (bmPlay.HitObjects[showHitObj.front].type & 2) { // slider
				int duration = bmPlay.HitObjects[showHitObj.front].sliders.pixelLength / (100.0 * bmPlay.Difficulty.SliderMultiplier) * mspb;
				endTime = bmPlay.HitObjects[showHitObj.front].time + duration * bmPlay.HitObjects[showHitObj.front].sliders.repeat + hitWinWidth.s50;
			}
			else if (bmPlay.HitObjects[showHitObj.front].type & 8) { // spinner
				endTime = bmPlay.HitObjects[showHitObj.front].endTime;
			}

			if (time > endTime) {
				showHitObj.front++;
				combo = 0;
			}
			else {
				break;
			}
		}


		cur.update();

		Mutex.lock();
		std::deque<Object::Container*>::iterator it = objs.begin();
		while (it != objs.end()) {
			(**it).update();

			if ((**it).willBeRemove) {
				delete *it;  // Because I use new for Object::Circle
				it = objs.erase(it);
			}
			else ++it;
		}
		Mutex.unlock();
	}

	void draw() {
		Mutex.lock();
		for (int i = 0; i < 5; i++) {  //Loop for select layer
			Object::Container::renderLayer = i;
			std::deque<Object::Container*>::reverse_iterator rit = objs.rbegin();
			std::deque<Object::Container*>::reverse_iterator rend = objs.rend();
			while (rit != rend) {
				m_window.draw(**rit, transform);
				++rit;
			}
		}
		Mutex.unlock();


		m_window.draw(cur);

	}

	void newEvent(sf::Event event) {
		switch (event.type) {
		case sf::Event::KeyPressed:
		case sf::Event::MouseButtonPressed:
			OnPressed(event);
			break;

		case sf::Event::KeyReleased:
		case sf::Event::MouseButtonReleased:
			OnReleased(event);
			break;
		}
	}

};