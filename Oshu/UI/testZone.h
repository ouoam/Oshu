#pragma once

#include <iostream>
#include <deque>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "../Beatmap/Beatmap.h"
#include "../Audio/hitsound.h"
#include "../Object/Container.h"
#include "../Object/Cursor.h"
#include "../Object/Circle.h"



namespace UI {

Object::Cursor *cur;

std::deque<Object::Container*> objs;

sf::Mutex Mutex;

struct A {
	sf::RenderWindow& m_window;
	A(sf::RenderWindow& window) : m_window(window) {}
}*win;

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


void load(sf::RenderWindow& window) {
	win = new A(window);
	cur = new Object::Cursor(window);


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
	else if (bmPlay.Difficulty.ApproachRate > 5) {
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
}

void OnPressed(sf::Event event) {
	cur->onMouseDown(event.key.code);
}

void OnReleased(sf::Event event) {
	cur->onMouseUp(event.key.code);
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

	// set new hit obj to show
	while (showHitObj.back < bmPlay.HitObjects.size()) {
		if (bmPlay.HitObjects[showHitObj.back].time - Beatmap::bmHitObjects::TimePreempt <= time) {
			showHitObj.back++;

			if (bmPlay.HitObjects[showHitObj.front].type & 1 || bmPlay.HitObjects[showHitObj.front].type & 2) { // circle
				Object::Circle *newCircle = new Object::Circle(&bmPlay.HitObjects[showHitObj.front]);
				newCircle->StartPreemptState();

				Mutex.lock();
				objs.push_back(newCircle);
				Mutex.unlock();
			}
		}
		else break;
	}

	while (showHitObj.front < bmPlay.HitObjects.size()) {
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


	cur->update();

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
	std::deque<Object::Container*>::reverse_iterator rit = objs.rbegin();
	for (rit = objs.rbegin(); rit != objs.rend(); ++rit)
		win->m_window.draw(**rit, transform);
	Mutex.unlock();


	win->m_window.draw(*cur);

}

}