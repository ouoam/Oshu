#include <Time.h>
#include <string>
#include <fstream>
#include <deque>
#include <cmath>
#include <sys/stat.h>

#include <iostream>

#include <SFML/Graphics.hpp>
//#include <SFML/Audio.hpp>

//#include "Beatmap/Beatmap.h"
//#include "Audio/hitsound.h"

#include "Object/Animate/Animate.h"


int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(800, 640), "SFML works!", sf::Style::Default, settings);
	window.setMouseCursorVisible(false);
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);

	////load beatmap
	//std::string base_dir = "resource/499488 Kana Nishino - Sweet Dreams (11t dnb mix)/";
	//Beatmap::Beatmap bmPlay(base_dir + "Kana Nishino - Sweet Dreams (11t dnb mix) (Ascendance) [ReFaller's Hard].osu");
	////std::string base_dir = "resource/150945 Knife Party - Centipede/";
	////Beatmap::Beatmap bmPlay(base_dir + "Knife Party - Centipede (Sugoi-_-Desu) [This isn't a map, just a simple visualisation].osu");
	//loadHitSound(&bmPlay, base_dir);
	//

	//float mspb = 0;
	//float oldmspb = 0;

	//int lastTimePass = 0;
	//int keyPressed = 0;
	//int nextHit = -1;
	//int lastHit = -1;
	//int hit = 0;
	//int combo = 0;
	//int lastDelay = 0;
	//int dist = 0;

	//std::string last = "";


	//struct {
	//	int s50, s100, s300;
	//} hitWinWidth;

	//hitWinWidth.s50 = 150 - 50 * (5 - bmPlay.Difficulty.OverallDifficulty) / 5;
	//hitWinWidth.s100 = 100 - 40 * (5 - bmPlay.Difficulty.OverallDifficulty) / 5;
	//hitWinWidth.s300 = 50 - 30 * (5 - bmPlay.Difficulty.OverallDifficulty) / 5;

	//struct {
	//	int front = 0;
	//	int back = 0;
	//} showHitObj;

	//// Calc For Hit Object
	//int preempt = 1200;
	//int fade_in = 800;
	//int AR = bmPlay.Difficulty.ApproachRate;

	//if (AR < 5) {
	//	preempt = 1200 + 600 * (5 - AR) / 5;
	//	fade_in = 800 + 400 * (5 - AR) / 5;
	//}
	//else if (bmPlay.Difficulty.ApproachRate > 5) {
	//	preempt = 1200 - 750 * (AR - 5) / 5;
	//	fade_in = 800 - 500 * (AR - 5) / 5;
	//}

	//int CircleRadius = bmPlay.Difficulty.CircleRadius;
	//// End Calc For Hit Object

	//sf::Transform transform;
	//transform.translate(80, 80);

	//sf::Music music;
	//if (!music.openFromFile("resource/audio.wav"))
	////if (!music.openFromFile("resource/150945 Knife Party - Centipede/02-knife_party-centipede.wav"))
	//	return -1; // error
	//music.setVolume(70);
	////music.setPlayingOffset(sf::seconds(52));
	//music.play();

	// Declare and load a texture
	sf::Texture texture;
	texture.loadFromFile("resource/Modify/cursor.png");
	texture.setSmooth(true);
	// Create a sprite
	Object::Animate::Animate sprite;
	sprite.setTexture(texture);
	//sprite.setTextureRect(sf::IntRect(10, 10, 50, 30));
	//sprite.setColor(sf::Color(255, 255, 255, 200));
	//sprite.setPosition(100, 25);

	//sprite.moveTo(sf::Vector2f(400, 320), sf::seconds(2))->fadeTo(0,sf::seconds(6));

	sprite.moveTo(sf::Vector2f(400, 320), 2000, Object::Animate::OutBounce)
		.scaleTo(sf::Vector2f(3, 3), 1500, Object::Animate::InElastic)
		.then()
		.fadeTo(0, 2000, Object::Animate::OutBounce)
		.then()
		.fadeTo(255);

	while (window.isOpen())
	{
		//int64_t time = music.getPlayingOffset().asMilliseconds();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		//// set volume of sound effect
		//if (bmPlay.iTimingPoints < bmPlay.TimingPoints.size()) {
		//	if (bmPlay.TimingPoints[bmPlay.iTimingPoints].Offset <= time) {
		//		for (int i = 0; i < 10; i++) {
		//			sound[i].setVolume(bmPlay.TimingPoints[bmPlay.iTimingPoints].Volume);
		//		}
		//		if (bmPlay.TimingPoints[bmPlay.iTimingPoints].mspb > 0) {
		//			mspb = bmPlay.TimingPoints[bmPlay.iTimingPoints].mspb;
		//			oldmspb = mspb;
		//		}
		//		else {
		//			mspb = oldmspb * -(bmPlay.TimingPoints[bmPlay.iTimingPoints].mspb) / 100;
		//		}

		//		bmPlay.iTimingPoints++;
		//	}
		//}

		//
		//if (showHitObj.front < bmPlay.HitObjects.size() && nextHit == -1) {
		//	if (bmPlay.HitObjects[showHitObj.front].time <= time) {
		//		if (bmPlay.HitObjects[showHitObj.front].type & 1) { // circle
		//			nextHit = bmPlay.HitObjects[showHitObj.front].time + 70;
		//			showHitObj.front++;
		//		}
		//		else if(bmPlay.HitObjects[showHitObj.front].type & 2) { // slider
		//			int duration = bmPlay.HitObjects[showHitObj.front].sliders.pixelLength / (100.0 * bmPlay.Difficulty.SliderMultiplier) * mspb;
		//			int calcTime = bmPlay.HitObjects[showHitObj.front].time;
		//			int temp = 0;
		//			int i = 0;
		//			while (temp <= (int)time) {
		//				temp = calcTime + 70;
		//				calcTime += duration;
		//				i++;
		//			}
		//			if (i > bmPlay.HitObjects[showHitObj.front].sliders.repeat +1 ) {
		//				showHitObj.front++;
		//			}
		//			else {
		//				nextHit = temp;
		//			}
		//		}
		//		else if (bmPlay.HitObjects[showHitObj.front].type & 8) { // spinner
		//			nextHit = bmPlay.HitObjects[showHitObj.front].endTime + 70;
		//			showHitObj.front++;
		//		}
		//	}
		//}
		//

		//if (time >= nextHit && nextHit != -1)
		//{
		//	/*int sampleset = bmPlay.TimingPoints[bmPlay.iTimingPoints - 1].SampleSet;
		//	int sound_id = bmPlay.HitObjects[showHitObj.front - (showHitObj.front == 0 ? 0 : 1)].hitSound;
		//	int index = bmPlay.TimingPoints[bmPlay.iTimingPoints - 1].SampleIndex;
		//	sound_id = (sound_id & 8) ? 3 : ((sound_id & 4) ? 2 : ((sound_id & 2) ? 1 : 0));
		//	sound[iSound % 10].setBuffer(hitSoundList[sampleset][sound_id][index]);
		//	sound[iSound % 10].play();
		//	sound[iSound % 10].setPlayingOffset(sf::seconds(0));
		//	iSound++;*/
		//	
		//	lastHit = nextHit;
		//	nextHit = -1;
		//	combo++;
		//}




		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
		//{
		//	if (keyPressed == 0)
		//	{
		//		if (showHitObj.front < bmPlay.HitObjects.size()) {
		//			sf::Vector2i position = sf::Mouse::getPosition(window);
		//			int hit = 0;
		//			int isHO = showHitObj.front;
		//			while (hit == 0 && isHO < showHitObj.back) {
		//				int hitTime = bmPlay.HitObjects[isHO].time;
		//				int hitDelay = time - hitTime - 20;
		//				lastDelay = hitDelay;
		//				position -= bmPlay.HitObjects[isHO].position;
		//				position -= sf::Vector2i(80, 80);

		//				int distant = sqrt((position.x * position.x) + (position.y * position.y));
		//				dist = distant;

		//				if (distant <= CircleRadius) {
		//					if (-hitWinWidth.s50 <= hitDelay && hitDelay <= hitWinWidth.s50) {
		//						last = " 50";
		//						if (-hitWinWidth.s100 <= hitDelay && hitDelay <= hitWinWidth.s100) {
		//							last = "100";
		//							if (-hitWinWidth.s300 <= hitDelay && hitDelay <= hitWinWidth.s300) {
		//								last = "300";
		//							}
		//						}
		//						
		//						int sampleset = bmPlay.TimingPoints[bmPlay.iTimingPoints].SampleSet;
		//						int sound_id = bmPlay.HitObjects[isHO].hitSound;
		//						int index = bmPlay.TimingPoints[bmPlay.iTimingPoints].SampleIndex;
		//						sound_id = (sound_id & 8) ? 3 : ((sound_id & 4) ? 2 : ((sound_id & 2) ? 1 : 0));

		//						sound[iSound % 10].setBuffer(hitSoundList[sampleset][sound_id][index]);
		//						sound[iSound % 10].play();
		//						sound[iSound % 10].setPlayingOffset(sf::seconds(0));
		//						iSound++;
		//						

		//						showHitObj.front = isHO + 1;
		//						combo++;

		//						

		//						hit = 1;
		//					}
		//					else {
		//						showHitObj.front++;
		//					}
		//				}

		//				isHO++;
		//			}
		//		}
		//	}
		//	keyPressed = 1;
		//}
		//else
		//	keyPressed = 0;



		//while (showHitObj.front < bmPlay.HitObjects.size())
		//{
		//	int endTime = 0;
		//	if (bmPlay.HitObjects[showHitObj.front].type & 1) { // circle
		//		endTime = bmPlay.HitObjects[showHitObj.front].time + hitWinWidth.s50; /////////////////////////// 
		//	}
		//	else if (bmPlay.HitObjects[showHitObj.front].type & 2) { // slider
		//		int duration = bmPlay.HitObjects[showHitObj.front].sliders.pixelLength / (100.0 * bmPlay.Difficulty.SliderMultiplier) * mspb;
		//		endTime = bmPlay.HitObjects[showHitObj.front].time + duration * bmPlay.HitObjects[showHitObj.front].sliders.repeat + hitWinWidth.s50;
		//	}
		//	else if (bmPlay.HitObjects[showHitObj.front].type & 8) { // spinner
		//		endTime = bmPlay.HitObjects[showHitObj.front].endTime;
		//	}

		//	if (time > endTime)
		//	{
		//		showHitObj.front++;
		//		combo = 0;
		//		last = "Miss";
		//	}
		//	else {
		//		break;
		//	}
		//}



		//// set new hit obj to show
		//while (showHitObj.back < bmPlay.HitObjects.size()) {
		//	if (bmPlay.HitObjects[showHitObj.back].time - preempt <= time)
		//		showHitObj.back++;
		//	else break;
		//}


		////render

		//window.clear(sf::Color::Black);

		//int iHO = 0;
		//// Hit Object
		//iHO = showHitObj.front;
		//while (iHO < showHitObj.back) {
		//	int opacity = 0;
		//	if (bmPlay.HitObjects[iHO].time - preempt + fade_in <= time) {
		//		opacity = 255;
		//	}
		//	else {
		//		opacity = ((time)-(bmPlay.HitObjects[iHO].time - preempt)) / (float)fade_in * 255.0;
		//	}
		//	float scale = (bmPlay.HitObjects[iHO].time - time) / (float)preempt * (4 - 1.1) + (1.1);

		//	opacity = opacity / 255.0 * 128.0;

		//	if (bmPlay.HitObjects[iHO].type & 2) {

		//		window.draw(bmPlay.HitObjects[iHO].sliders.curvePoints, transform);

		//		sf::VertexArray line[2];
		//		line[0].setPrimitiveType(sf::LineStrip);
		//		line[1].setPrimitiveType(sf::LineStrip);
		//		int n = bmPlay.HitObjects[iHO].sliders.lineThinkness.getVertexCount();
		//		for (int i = 0; i < n; i++) {
		//			bmPlay.HitObjects[iHO].sliders.lineThinkness[i].color = sf::Color(255, 255, 255, opacity);
		//			line[i % 2].append(bmPlay.HitObjects[iHO].sliders.lineThinkness[i]);
		//		}
		//		window.draw(bmPlay.HitObjects[iHO].sliders.lineThinkness, transform);

		//		window.draw(line[0], transform);
		//		window.draw(line[1], transform);

		//		int duration = bmPlay.HitObjects[showHitObj.front].sliders.pixelLength / (100.0 * bmPlay.Difficulty.SliderMultiplier) * mspb;
		//		int endTime = bmPlay.HitObjects[showHitObj.front].time + duration * bmPlay.HitObjects[showHitObj.front].sliders.repeat;

		//		if (bmPlay.HitObjects[iHO].time <= time && time <= endTime) {
		//			int nowT = time - bmPlay.HitObjects[iHO].time;
		//			float duration = bmPlay.HitObjects[iHO].sliders.pixelLength / (100.0 * bmPlay.Difficulty.SliderMultiplier) * mspb;
		//			int n = bmPlay.HitObjects[iHO].sliders.curvePoints.getVertexCount();

		//			float tem = 0;
		//			if ((int)(nowT / duration) % 2 == 0)
		//				tem = fmod(nowT, duration);
		//			else
		//				tem = duration - fmod(nowT, duration);

		//			float maxLength = 0;
		//			for (int i = 1; i < n; i++) {
		//				sf::Vector2f a = bmPlay.HitObjects[iHO].sliders.curvePoints[i].position - bmPlay.HitObjects[iHO].sliders.curvePoints[i - 1].position;
		//				maxLength += sqrt((a.x*a.x) + (a.y*a.y));
		//			}

		//			float nowL = maxLength * (tem / duration);
		//			float length = 0;
		//			sf::Vector2f out;
		//			for (int i = 1; i < n; i++) {
		//				sf::Vector2f a = bmPlay.HitObjects[iHO].sliders.curvePoints[i].position - bmPlay.HitObjects[iHO].sliders.curvePoints[i - 1].position;
		//				float addl = sqrt((a.x*a.x) + (a.y*a.y));
		//				if (length + addl < nowL) {
		//					length += addl;
		//				}
		//				else {
		//					float gol = (nowL - length) / addl;
		//					out = bmPlay.HitObjects[iHO].sliders.curvePoints[i - 1].position + a * gol;
		//					break;
		//				}
		//			}

		//			sf::CircleShape circle(CircleRadius * 0.9);
		//			circle.setFillColor(sf::Color(255, 0, 0, opacity));
		//			circle.setOrigin(CircleRadius * 0.9, CircleRadius * 0.9);
		//			circle.setFillColor(sf::Color(255, 255, 255, 0));
		//			circle.setOutlineThickness(3);
		//			circle.setOutlineColor(sf::Color(255, 255, 0, opacity));
		//			circle.setPosition(out);
		//			window.draw(circle, transform);

		//			sf::CircleShape circle1(CircleRadius * 2);
		//			circle1.setFillColor(sf::Color(255, 0, 0, opacity));
		//			circle1.setOrigin(CircleRadius * 2, CircleRadius * 2);
		//			circle1.setFillColor(sf::Color(255, 255, 255, 0));
		//			circle1.setOutlineThickness(3);
		//			circle1.setOutlineColor(sf::Color(255, 255, 0, opacity / 2.0));
		//			circle1.setPosition(out);
		//			window.draw(circle1, transform);
		//		}

		//		sf::CircleShape hitcircle(CircleRadius);
		//		hitcircle.setFillColor(sf::Color(255, 0, 0, opacity));
		//		hitcircle.setOrigin(CircleRadius, CircleRadius);
		//		hitcircle.setPosition(bmPlay.HitObjects[iHO].sliders.endPoint);
		//		window.draw(hitcircle, transform);

		//	}

		//	sf::CircleShape hitcircle(CircleRadius);
		//	hitcircle.setFillColor(sf::Color(255, 0, 0, opacity));
		//	hitcircle.setOrigin(CircleRadius, CircleRadius);
		//	hitcircle.setPosition((sf::Vector2f) bmPlay.HitObjects[iHO].position);
		//	window.draw(hitcircle, transform);

		//	if (scale > 1.1) {
		//		sf::CircleShape approachcircle(CircleRadius);
		//		approachcircle.setOrigin(CircleRadius, CircleRadius);
		//		approachcircle.setScale(scale, scale);
		//		approachcircle.setFillColor(sf::Color(255, 255, 255, 0));
		//		approachcircle.setOutlineThickness(3);
		//		approachcircle.setOutlineColor(sf::Color(255, 255, 255, opacity));
		//		approachcircle.setPosition((sf::Vector2f) bmPlay.HitObjects[iHO].position);
		//		window.draw(approachcircle, transform);
		//	}

		//	iHO++;
		//}
		//// End Hit Object

		//

		//// For debug

		//sf::Text text;
		//sf::Font font;
		//font.loadFromFile("arial.ttf");
		//text.setFont(font);
		//text.setString(std::to_string(lastDelay));
		//text.setCharacterSize(16);
		//text.setFillColor(sf::Color::Red);
		//window.draw(text);

		//sf::Text text1;
		//text1.setFont(font);
		//text1.setString(last);
		//text1.setCharacterSize(16);
		//text1.setFillColor(sf::Color::Red);
		//text1.setPosition(0, 20);
		//window.draw(text1);

		//sf::Text text2;
		//text2.setFont(font);
		//text2.setString(std::to_string(combo) + "x");
		//text2.setCharacterSize(16);
		//text2.setFillColor(sf::Color::Red);
		//text2.setPosition(0, 40);
		//window.draw(text2);

		//sf::Vector2i localPosition = sf::Mouse::getPosition(window);
		//sf::CircleShape mouse(15);
		//mouse.setOrigin(15, 15);
		//mouse.setFillColor(sf::Color(118, 198, 212));
		//mouse.setOutlineThickness(2.5);
		//mouse.setOutlineColor(sf::Color(255, 255, 255));
		//mouse.setPosition((sf::Vector2f) localPosition);
		//window.draw(mouse);

		//sf::Text text3;
		//text3.setFont(font);
		//text3.setString("mspb = " + std::to_string(mspb));
		//text3.setCharacterSize(16);
		//text3.setFillColor(sf::Color::Red);
		//text3.setPosition(0, 60);
		//window.draw(text3);

		//sf::Text text4;
		//text4.setFont(font);
		//text4.setString("y = " + std::to_string(localPosition.y));
		//text4.setCharacterSize(16);
		//text4.setFillColor(sf::Color::Red);
		//text4.setPosition(0, 80);
		//window.draw(text4);

		//sf::Text text5;
		//text5.setFont(font);
		//text5.setString("dist = " + std::to_string(dist));
		//text5.setCharacterSize(16);
		//text5.setFillColor(sf::Color::Red);
		//text5.setPosition(0, 100);
		//window.draw(text5);
		
		sprite.update();

		window.clear(sf::Color::Black);
		window.draw(sprite);
		window.display();
	}

	return 0;
}
