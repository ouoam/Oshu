#pragma once

#include <SFML/Graphics.hpp>

#include "../Object/Cursor.h"
#include "../Beatmap/Beatmap.h"
#include "../Audio/hitsound.h"

namespace UI {
	Object::Cursor *cur;

	float mspb = 0;
	float oldmspb = 0;

	int lastTimePass = 0;
	int keyPressed = 0;
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

	int preempt = 1200;
	int fade_in = 800;
	int AR;

	int CircleRadius;

	sf::Transform transform;

	std::string last = "";

	sf::Music music;
	Beatmap::Beatmap bmPlay;

	struct A
	{
		sf::RenderWindow& m_window;
		A(sf::RenderWindow& window) : m_window(window) {}
	}*win;
	

	void load(sf::RenderWindow& window) {
		win = new A(window);
		cur = new Object::Cursor(window);

		//load beatmap
		std::string base_dir = "resource/499488 Kana Nishino - Sweet Dreams (11t dnb mix)/";
		bmPlay.load(base_dir + "Kana Nishino - Sweet Dreams (11t dnb mix) (Ascendance) [ReFaller's Hard].osu");
		//std::string base_dir = "resource/150945 Knife Party - Centipede/";
		//Beatmap::Beatmap bmPlay(base_dir + "Knife Party - Centipede (Sugoi-_-Desu) [This isn't a map, just a simple visualisation].osu");
		loadHitSound(&bmPlay, base_dir);

		hitWinWidth.s50 = 150 - 50 * (5 - bmPlay.Difficulty.OverallDifficulty) / 5;
		hitWinWidth.s100 = 100 - 40 * (5 - bmPlay.Difficulty.OverallDifficulty) / 5;
		hitWinWidth.s300 = 50 - 30 * (5 - bmPlay.Difficulty.OverallDifficulty) / 5;

		// Calc For Hit Object
		preempt = 1200;
		fade_in = 800;
		AR = bmPlay.Difficulty.ApproachRate;

		if (AR < 5) {
			preempt = 1200 + 600 * (5 - AR) / 5;
			fade_in = 800 + 400 * (5 - AR) / 5;
		}
		else if (bmPlay.Difficulty.ApproachRate > 5) {
			preempt = 1200 - 750 * (AR - 5) / 5;
			fade_in = 800 - 500 * (AR - 5) / 5;
		}

		CircleRadius = bmPlay.Difficulty.CircleRadius;
		// End Calc For Hit Object

		
		transform.translate(80, 60);

		if (!music.openFromFile("resource/audio.wav"))
		//if (!music.openFromFile("resource/150945 Knife Party - Centipede/02-knife_party-centipede.wav"))
			return ; // error
		music.setVolume(70);
		//music.setPlayingOffset(sf::seconds(52));
		music.play();

	}

	void OnPressed(sf::Event event) {
		cur->OnPressed(event.key.code);
	}

	void OnReleased(sf::Event event) {
		cur->OnReleased(event.key.code);
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
			if (bmPlay.HitObjects[showHitObj.back].time - preempt <= time)
				showHitObj.back++;
			else break;
		}

		while (showHitObj.front < bmPlay.HitObjects.size())
		{
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

			if (time > endTime)
			{
				showHitObj.front++;
				combo = 0;
				last = "Miss";
			}
			else {
				break;
			}
		}

		cur->update();
	}

	void draw() {
		int64_t time = music.getPlayingOffset().asMilliseconds();

		int iHO = 0;
		// Hit Object
		iHO = showHitObj.front;
		while (iHO < showHitObj.back) {
			int opacity = 0;
			if (bmPlay.HitObjects[iHO].time - preempt + fade_in <= time) {
				opacity = 255;
			}
			else {
				opacity = ((time)-(bmPlay.HitObjects[iHO].time - preempt)) / (float)fade_in * 255.0;
			}
			float scale = (bmPlay.HitObjects[iHO].time - time) / (float)preempt * (4 - 1.1) + (1.1);

			opacity = opacity / 255.0 * 128.0;

			if (bmPlay.HitObjects[iHO].type & 2) {

				win->m_window.draw(bmPlay.HitObjects[iHO].sliders.curvePoints, transform);

				sf::VertexArray line[2];
				line[0].setPrimitiveType(sf::LineStrip);
				line[1].setPrimitiveType(sf::LineStrip);
				int n = bmPlay.HitObjects[iHO].sliders.lineThinkness.getVertexCount();
				for (int i = 0; i < n; i++) {
					bmPlay.HitObjects[iHO].sliders.lineThinkness[i].color = sf::Color(255, 255, 255, opacity);
					line[i % 2].append(bmPlay.HitObjects[iHO].sliders.lineThinkness[i]);
				}
				win->m_window.draw(bmPlay.HitObjects[iHO].sliders.lineThinkness, transform);

				win->m_window.draw(line[0], transform);
				win->m_window.draw(line[1], transform);

				int duration = bmPlay.HitObjects[showHitObj.front].sliders.pixelLength / (100.0 * bmPlay.Difficulty.SliderMultiplier) * mspb;
				int endTime = bmPlay.HitObjects[showHitObj.front].time + duration * bmPlay.HitObjects[showHitObj.front].sliders.repeat;

				if (bmPlay.HitObjects[iHO].time <= time && time <= endTime) {
					int nowT = time - bmPlay.HitObjects[iHO].time;
					float duration = bmPlay.HitObjects[iHO].sliders.pixelLength / (100.0 * bmPlay.Difficulty.SliderMultiplier) * mspb;
					int n = bmPlay.HitObjects[iHO].sliders.curvePoints.getVertexCount();

					float tem = 0;
					if ((int)(nowT / duration) % 2 == 0)
						tem = fmod(nowT, duration);
					else
						tem = duration - fmod(nowT, duration);

					float maxLength = 0;
					for (int i = 1; i < n; i++) {
						sf::Vector2f a = bmPlay.HitObjects[iHO].sliders.curvePoints[i].position - bmPlay.HitObjects[iHO].sliders.curvePoints[i - 1].position;
						maxLength += sqrt((a.x*a.x) + (a.y*a.y));
					}

					float nowL = maxLength * (tem / duration);
					float length = 0;
					sf::Vector2f out;
					for (int i = 1; i < n; i++) {
						sf::Vector2f a = bmPlay.HitObjects[iHO].sliders.curvePoints[i].position - bmPlay.HitObjects[iHO].sliders.curvePoints[i - 1].position;
						float addl = sqrt((a.x*a.x) + (a.y*a.y));
						if (length + addl < nowL) {
							length += addl;
						}
						else {
							float gol = (nowL - length) / addl;
							out = bmPlay.HitObjects[iHO].sliders.curvePoints[i - 1].position + a * gol;
							break;
						}
					}

					sf::CircleShape circle(CircleRadius * 0.9);
					circle.setFillColor(sf::Color(255, 0, 0, opacity));
					circle.setOrigin(CircleRadius * 0.9, CircleRadius * 0.9);
					circle.setFillColor(sf::Color(255, 255, 255, 0));
					circle.setOutlineThickness(3);
					circle.setOutlineColor(sf::Color(255, 255, 0, opacity));
					circle.setPosition(out);
					win->m_window.draw(circle, transform);

					sf::CircleShape circle1(CircleRadius * 2);
					circle1.setFillColor(sf::Color(255, 0, 0, opacity));
					circle1.setOrigin(CircleRadius * 2, CircleRadius * 2);
					circle1.setFillColor(sf::Color(255, 255, 255, 0));
					circle1.setOutlineThickness(3);
					circle1.setOutlineColor(sf::Color(255, 255, 0, opacity / 2.0));
					circle1.setPosition(out);
					win->m_window.draw(circle1, transform);
				}

				sf::CircleShape hitcircle(CircleRadius);
				hitcircle.setFillColor(sf::Color(255, 0, 0, opacity));
				hitcircle.setOrigin(CircleRadius, CircleRadius);
				hitcircle.setPosition(bmPlay.HitObjects[iHO].sliders.endPoint);
				win->m_window.draw(hitcircle, transform);

			}

			sf::CircleShape hitcircle(CircleRadius);
			hitcircle.setFillColor(sf::Color(255, 0, 0, opacity));
			hitcircle.setOrigin(CircleRadius, CircleRadius);
			hitcircle.setPosition((sf::Vector2f) bmPlay.HitObjects[iHO].position);
			win->m_window.draw(hitcircle, transform);

			if (scale > 1.1) {
				sf::CircleShape approachcircle(CircleRadius);
				approachcircle.setOrigin(CircleRadius, CircleRadius);
				approachcircle.setScale(scale, scale);
				approachcircle.setFillColor(sf::Color(255, 255, 255, 0));
				approachcircle.setOutlineThickness(3);
				approachcircle.setOutlineColor(sf::Color(255, 255, 255, opacity));
				approachcircle.setPosition((sf::Vector2f) bmPlay.HitObjects[iHO].position);
				win->m_window.draw(approachcircle, transform);
			}

			iHO++;
		}
		// End Hit Object


		cur->draw();
	}
}