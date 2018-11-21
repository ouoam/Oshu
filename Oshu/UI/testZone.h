#pragma once

#include <iostream>
#include <deque>
#include <cmath>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <sfeMovie/Movie.hpp>

#include "../Beatmap/Beatmap.h"
#include "../Beatmap/Difficulty.h"
#include "../Audio/hitsound.h"
#include "../Object/Container.h"
#include "../Object/Cursor.h"
#include "../Object/Circle.h"
#include "../Object/Slider.h"

#include "../Object/HitWindows.h"
#include "../Scoring/HitResult.h"

#include "../Scoring/HitResult.h"

#include "UI.h"

class testUI : public UI {

	Object::Cursor cur;

	std::deque<Object::ContainerHitObject*> objs;

	std::unordered_map<std::string, std::string> beatmapData;
	sfe::Movie *playSong;

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

	Object::HitWindows hitwindows;

	struct {
		int front = 0;
		int back = 0;
	} showHitObj;

	bool haveStart = false;
	
	Beatmap::Beatmap bmPlay;


	int aaa = 0;
	int64_t oldTime = 0;

protected:


	void OnPressed(sf::Event event) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code != sf::Keyboard::A && event.key.code != sf::Keyboard::S)
				return;
		}
		cur.onMouseDown(event.key.code);

		Mutex.lock();

		int64_t time = playSong->getPlayingOffset().asMilliseconds();
		sf::Vector2f click = sf::Vector2f(sf::Mouse::getPosition(m_window));
		//sf::Vector2f click = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

		for (Object::ContainerHitObject* obj : objs) {
			
			if (obj->canClick) {
				sf::Vector2f offset = click - transform.transformPoint(sf::Vector2f(obj->hitObject->position));

				float dist = sqrt(offset.x * offset.x + offset.y * offset.y);

				if (dist <= (obj->hitObject->CR) / 2) {
					std::cout << hitwindows.ResultFor(time - obj->hitObject->time) << "\t" << time << "\t" << obj->hitObject->time << "\t" << Beatmap::bmHitObjects::TimePreempt << "hit\n";
					obj->onMouseClick(event.key.code);
					oldTime = obj->hitObject->time;

					break;
				}
			}
		}

		Mutex.unlock();
	}

	void OnReleased(sf::Event event) {
		cur.onMouseUp(event.key.code);
	}


	void onDelete() {
		for (auto obj : objs)
			delete obj;
	}


	void onUpdate() {
		cur.update();

		if (!haveStart) {
			playSong->stop();
			playSong->play();
			haveStart = true;
		}

		if (playSong->getStatus() == sfe::Status::Paused || playSong->getStatus() == sfe::Status::Stopped)
			return;

		int64_t time = playSong->getPlayingOffset().asMilliseconds();

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
		while (showHitObj.front + 1 < HOsize) {
			if (bmPlay.HitObjects[showHitObj.front + 1].time - Beatmap::bmHitObjects::TimePreempt <= time) {
				showHitObj.front++;

				if (bmPlay.HitObjects[showHitObj.front].type & 1) { // circle
					Object::Circle *newCircle = new Object::Circle(&bmPlay.HitObjects[showHitObj.front]);
					newCircle->StartPreemptState();

					Mutex.lock();
					objs.push_back(newCircle);
					Mutex.unlock();
				}
				else if (bmPlay.HitObjects[showHitObj.front].type & 2) { // slider
					Object::Slider *newSlider = new Object::Slider(&bmPlay.HitObjects[showHitObj.front]);
					newSlider->StartPreemptState();

					Mutex.lock();
					objs.push_back(newSlider);
					Mutex.unlock();
				}
			}
			else break;
		}

		//while (showHitObj.front < HOsize) {
		//	int endTime = 0;
		//	if (bmPlay.HitObjects[showHitObj.front].type & 1) { // circle
		//		endTime = bmPlay.HitObjects[showHitObj.front].time + hitwindows.HalfWindowFor(Scoring::HitResult::Miss);
		//	}
		//	else if (bmPlay.HitObjects[showHitObj.front].type & 2) { // slider
		//		int duration = bmPlay.HitObjects[showHitObj.front].sliders.pixelLength / (100.0 * bmPlay.Difficulty.SliderMultiplier) * mspb;
		//		endTime = bmPlay.HitObjects[showHitObj.front].time + duration * bmPlay.HitObjects[showHitObj.front].sliders.repeat + hitwindows.HalfWindowFor(Scoring::HitResult::Miss);
		//	}
		//	else if (bmPlay.HitObjects[showHitObj.front].type & 8) { // spinner
		//		endTime = bmPlay.HitObjects[showHitObj.front].endTime;
		//	}

		//	if (time > endTime) {
		//		showHitObj.front++;
		//		combo = 0;
		//	}
		//	else {
		//		break;
		//	}
		//}


		Mutex.lock();
		std::deque<Object::ContainerHitObject*>::iterator it = objs.begin();
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

	void onDraw() {
		Mutex.lock();
		for (int i = 0; i < 5; i++) {  //Loop for select layer
			Object::Container::renderLayer = i;
			std::deque<Object::ContainerHitObject*>::reverse_iterator rit = objs.rbegin();
			std::deque<Object::ContainerHitObject*>::reverse_iterator rend = objs.rend();
			while (rit != rend) {
				m_window.draw(**rit, transform);
				++rit;
			}
		}
		Mutex.unlock();

		m_window.draw(cur);
	}

	void onEvent(sf::Event event) {
		switch (event.type) {
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Escape)
				gobackUI();
			if (event.key.alt && event.key.control)
				m_window.setMouseCursorGrabbed(false);

		case sf::Event::MouseButtonPressed:
			OnPressed(event);
			break;

		case sf::Event::KeyReleased:
		case sf::Event::MouseButtonReleased:
			OnReleased(event);
			break;

		case sf::Event::GainedFocus:
			m_window.setMouseCursorGrabbed(true);
			break;
		}
	}


public:

	testUI(sf::RenderWindow& window, UI *from, std::unordered_map<std::string, std::string> bmData, sfe::Movie *playSong) :
		UI(window, from) , cur(window), beatmapData(bmData), playSong(playSong)
	{
		std::string base_dir = "D:/osu!/Songs/";
		base_dir += beatmapData["OsuDir"] + "/";

		bmPlay.load(base_dir + beatmapData["OsuFile"]);

		loadHitSound(&bmPlay, base_dir);

		// Calc For Hit Object
		int AR = bmPlay.Difficulty.ApproachRate;

		Beatmap::bmHitObjects::TimePreempt = Beatmap::Difficulty::Range(AR, 1800, 1200, 450);
		Beatmap::bmHitObjects::TimeFadeIn = Beatmap::Difficulty::Range(AR, 1200, 800, 300);

		Beatmap::bmHitObjects::CR = bmPlay.Difficulty.CircleRadius;
		// End Calc For Hit Object

		transform.translate(80, 60);

		playSong->stop();

		hitwindows.SetDifficulty(bmPlay.Difficulty.OverallDifficulty);

		std::cout << hitwindows.HalfWindowFor(Scoring::HitResult::Great) << "\t"
			<< hitwindows.HalfWindowFor(Scoring::HitResult::Good) << "\t"
			<< hitwindows.HalfWindowFor(Scoring::HitResult::Meh) << "\t"
			<< hitwindows.HalfWindowFor(Scoring::HitResult::Miss) << std::endl;
		
		m_window.setKeyRepeatEnabled(false);
		m_window.setMouseCursorGrabbed(true);
	}
};