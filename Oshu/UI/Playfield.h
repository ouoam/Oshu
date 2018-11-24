#pragma once

#include <iostream>
#include <deque>
#include <cmath>
#include <thread>

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
#include "../Scoring/ScoreProcessor.h"

#include "UI.h"

class Playfield : public UI {

	DB::gameDB gameDB;

	Object::Cursor cur;

	std::deque<Object::ContainerHitObject*> objs;

	std::unordered_map<std::string, std::string> beatmapData;
	sfe::Movie playSong;
	sfe::Movie playVideo;
	sf::Texture playVideoTexture;
	myMutex playVideoTextureMutex;
	bool loadVideo = false;

	sf::RectangleShape filter;

	sf::Mutex Mutex;
	sf::Mutex MutexText;

	sf::Transform transform;

	Object::HitWindows hitwindows;

	struct {
		int front = 0;
		int back = 0;
	} showHitObj;

	int haveStart = 0;
	sf::Clock aaaaa;
	
	Beatmap::Beatmap bmPlay;

	Scoring::ScoreProcessor *scoreProcessor = nullptr;
	bool haveStoreScore = false;

	sf::Font font;
	sf::Text textScore;
	sf::Text textCombo;
	sf::Text textAccuracy;

	sf::Sprite Background;
	sf::Texture BackgroundTexture;

	void updateVideo() {
		updateVideoThreadRunningA = true;
		float framerate = playVideo.getFramerate();
		sf::Time m_frameTimeLimit = sf::seconds(1.f / framerate);
		sf::Clock m_clock;

		sf::Vector2f videoSize = playVideo.getSize();

		sf::RenderTexture videoTexture;
		if (!videoTexture.create(videoSize.x, videoSize.y)) {
			std::cout << "Error create render text" << std::endl;
		}
		while (updateVideoThreadRunning && isUIshow() && playVideo.getStatus() == sfe::Status::Playing) {
			updateVideoMutex.lock();
			playVideo.update();
			if (!updateVideoThreadRunning || !isUIshow() || playVideo.getStatus() != sfe::Status::Playing) {
				updateVideoMutex.unlock();
				break;
			}
			videoTexture.draw(playVideo);
			videoTexture.display();
			updateVideoMutex.unlock();

			playVideoTextureMutex.lock();
			playVideoTexture = videoTexture.getTexture();
			playVideoTextureMutex.unlock();
			

			sf::sleep(m_frameTimeLimit - m_clock.getElapsedTime());
			m_clock.restart();
		}
		updateVideoThreadRunningA = false;
	}

	myMutex updateVideoMutex;
	std::thread *updateVideoThread;
	bool updateVideoThreadRunning = false;
	bool updateVideoThreadRunningA = false;

protected:
	void OnPressed(sf::Event event) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code != sf::Keyboard::A && event.key.code != sf::Keyboard::S)
				return;
		}
		cur.onMouseDown(event.key.code);

		Mutex.lock();

		int64_t time = playSong.getPlayingOffset().asMilliseconds();
		sf::Vector2f click = sf::Vector2f(sf::Mouse::getPosition(m_window));

		for (Object::ContainerHitObject* obj : objs) {
			
			if (obj->canClick) {

				sf::Vector2f clickPos = transform.getInverse().transformPoint(click);

				sf::Vector2f offset = clickPos - (sf::Vector2f)obj->hitObject->position;

				float dist = sqrt(offset.x * offset.x + offset.y * offset.y);

				if (dist <= (obj->hitObject->CR) / 2) {
					int64_t offsetTime = time - obj->hitObject->time;
					Scoring::HitResult::Enum result = hitwindows.ResultFor(offsetTime);
					if (result == Scoring::HitResult::None) {
						//  do shake
						std::cout << offsetTime << "----------------------------------------------------------\n";
						break;
					} else {
						obj->onMouseClick(event.key.code);

						int sampleset = bmPlay.TimingPoints[std::max(bmPlay.iTimingPoints - 1, 0)].SampleSet;
						int sound_id = obj->hitObject->hitSound;
						int index = bmPlay.TimingPoints[bmPlay.iTimingPoints - 1].SampleIndex;
						sound_id = (sound_id & 8) ? 3 : ((sound_id & 4) ? 2 : ((sound_id & 2) ? 1 : 0));
						sound[iSound % 10].setBuffer(hitSoundList[sampleset][sound_id][index]);
						sound[iSound % 10].play();
						sound[iSound % 10].setPlayingOffset(sf::seconds(0));
						iSound++;

						scoreProcessor->ApplyResult(Scoring::JudgementResult(result));

						std::cout << "R:" << result << "\tS:" << scoreProcessor->TotalScore << "\tC:" << scoreProcessor->Combo << "\tR:" << scoreProcessor->Rank << "\tA:" << scoreProcessor->Accuracy << std::endl;

						break;
					}
				}
			}
		}

		Mutex.unlock();
	}

	void OnReleased(sf::Event event) {
		cur.onMouseUp(event.key.code);
	}

	virtual void onDelete() {
		playVideo.stop();
		updateVideoThreadRunning = false;
		updateVideoMutex.lock();
		playVideoTextureMutex.lock();

		for (auto obj : objs)
			delete obj;
		objs.clear();
		if (scoreProcessor != nullptr)
			delete scoreProcessor;
		//updateVideoThread->join();
		while (updateVideoThreadRunningA);
		//if (updateVideoThread != nullptr)
		//	delete updateVideoThread;
	}

	virtual void onUpdate() {
		playSong.update();
		cur.update();

		if (haveStart == 0) {
			playSong.stop();
			aaaaa.restart();
			haveStart++;
		} else if (haveStart == 1) {
			if (aaaaa.getElapsedTime().asMilliseconds() >= bmPlay.General.AudioLeadIn + 1000) {
				haveStart++;
			}
		} else if (haveStart == 2) {
			playSong.play();
			haveStart++;
		}

		if (loadVideo) {
			if (playVideo.getStatus() == sfe::Status::Stopped) {
				if (aaaaa.getElapsedTime().asMilliseconds() >= bmPlay.General.AudioLeadIn + 1000 + bmPlay.Events.VideoOffset) {
					if (!updateVideoThreadRunning) {
						playVideo.play();
						playVideo.update();
						updateVideoThreadRunning = true;
						updateVideoThread = new std::thread(&Playfield::updateVideo, this);
						updateVideoThread->detach();
					}
				}
			}
		}
		

		if ((*scoreProcessor).hasCompleted()) {
			if (!haveStoreScore) {
				std::cout << "END" << std::endl;
				Scoring::Score record;
				(*scoreProcessor).PopulateScore(&record);

				record.BeatmapID = std::stoi(beatmapData["id"]);
				gameDB.addScoreToDB(record);
				gobackUI();
				haveStoreScore = true;
			}
		}

		char buff[100];
		
		snprintf(buff, sizeof(buff), "%07.0lf", scoreProcessor->TotalScore);
		textScore.setString(buff);
		textCombo.setString(std::to_string(scoreProcessor->Combo) + "x");
		snprintf(buff, sizeof(buff), "%.2lf", scoreProcessor->Accuracy * 100);
		textAccuracy.setString(std::string(buff) + " %");


		if (playSong.getStatus() == sfe::Status::Paused || playSong.getStatus() == sfe::Status::Stopped) {
			
			return;
		}
			

		int64_t time = playSong.getPlayingOffset().asMilliseconds();

		updateHitsound(&bmPlay, time);

		int HOsize = bmPlay.HitObjects.size();

		// set new hit obj to show
		while (showHitObj.front + 1 < HOsize) {
			if (bmPlay.HitObjects[showHitObj.front + 1].time - Beatmap::bmHitObjects::TimePreempt <= time) {
				showHitObj.front++;

				if (bmPlay.HitObjects[showHitObj.front].type & 1) { // circle
					bmPlay.HitObjects[showHitObj.front].endTime = bmPlay.HitObjects[showHitObj.front].time + hitwindows.HalfWindowFor(Scoring::HitResult::Meh);
					Object::Circle *newCircle = new Object::Circle(&bmPlay.HitObjects[showHitObj.front]);
					newCircle->StartPreemptState();

					Mutex.lock();
					objs.push_back(newCircle);
					Mutex.unlock();
				}
				else if (bmPlay.HitObjects[showHitObj.front].type & 2) { // slider
					int duration = bmPlay.HitObjects[showHitObj.front].sliders.pixelLength / (100.0 * bmPlay.Difficulty.SliderMultiplier) * mspb;
					bmPlay.HitObjects[showHitObj.front].endTime = bmPlay.HitObjects[showHitObj.front].time + duration * bmPlay.HitObjects[showHitObj.front].sliders.repeat + hitwindows.HalfWindowFor(Scoring::HitResult::Miss);

					Object::Slider *newSlider = new Object::Slider(&bmPlay.HitObjects[showHitObj.front]);
					newSlider->StartPreemptState();

					Mutex.lock();
					objs.push_back(newSlider);
					Mutex.unlock();
				}
				else {
					scoreProcessor->ApplyResult(Scoring::HitResult::Great);
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

			if ((**it).hitObject->endTime <= time && (**it).canClick) {
				(**it).miss();
				(**it).canClick = false;
				scoreProcessor->ApplyResult(Scoring::JudgementResult(Scoring::HitResult::Miss));
			}

			if ((**it).willBeRemove) {
				delete *it;  // Because I use new for Object::Circle
				it = objs.erase(it);
			}
			else ++it;
		}
		Mutex.unlock();
	}

	virtual void onDraw() {
		m_window.draw(Background);
		playVideoTextureMutex.lock();
		m_window.draw(sf::Sprite(playVideoTexture));
		//m_window.draw(playVideo);
		playVideoTextureMutex.unlock();

		m_window.draw(filter);

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
		
		MutexText.lock();
		m_window.draw(textScore);
		m_window.draw(textCombo);
		m_window.draw(textAccuracy);
		MutexText.unlock();

		m_window.draw(cur);
	}

	virtual void onEvent(sf::Event event) {
		switch (event.type) {
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Escape)
				gobackUI();
			if (event.key.alt && event.key.control)
				m_window.setMouseCursorGrabbed(false);
			if (event.key.code == sf::Keyboard::Tilde)
				retry();

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

		case sf::Event::Resized:
			MutexText.lock();
			setScale();
			MutexText.unlock();
			break;
		}
	}

	void setScale() {
		sf::Vector2u winSize = m_window.getSize();
		double sx = (double)winSize.x / 800;
		double sy = (double)winSize.y / 600;

		double scale = std::min(sx, sy);

		transform = sf::Transform::Identity;
		transform.scale(scale, scale);
		transform.translate(((winSize.x - (800 * scale)) / 2) + (800 * scale * 0.1),
			                ((winSize.y - (600 * scale)) / 2) + (600 * scale * 0.1));

		
		textScore.setCharacterSize(18 * scale * 2.5);
		textScore.setString("0000000p");
		textScore.setOrigin(textScore.getGlobalBounds().width, 0);
		textScore.setPosition(sf::Vector2f(winSize.x - 10, 10));

		textCombo.setCharacterSize(18 * scale * 2.5);
		textCombo.setString("0x");
		textCombo.setOrigin(0, textCombo.getGlobalBounds().height);
		textCombo.setPosition(sf::Vector2f(10, winSize.y - 30));

		textAccuracy.setCharacterSize(12 * scale * 2.5);
		textAccuracy.setString("100.00 %");
		textAccuracy.setOrigin(textAccuracy.getGlobalBounds().width, 0);
		textAccuracy.setPosition(sf::Vector2f(winSize.x - 10, textScore.getGlobalBounds().height + 30));



		BackgroundTexture.setSmooth(true);
		Background.setTexture(BackgroundTexture);

		sf::Vector2u bgSize = BackgroundTexture.getSize();
		sx = (double)winSize.x / (double)bgSize.x;
		sy = (double)winSize.y / (double)bgSize.y;
		scale = std::max(sx, sy);

		Background.setOrigin(bgSize.x / 2.0, bgSize.y / 2.0);
		Background.setPosition(winSize.x / 2.0, winSize.y / 2.0);
		Background.setScale(scale, scale);

		sf::Rect<int> BackgroundRect(0, 0, bgSize.x, bgSize.y);
		Background.setTextureRect(BackgroundRect);

		//if (loadVideo) {
		//	bgSize = (sf::Vector2u)playVideo.getSize();

		//	sx = (double)winSize.x / (double)bgSize.x;
		//	sy = (double)winSize.y / (double)bgSize.y;
		//	scale = std::max(sx, sy);

		//	playVideo.fit(0, 0, bgSize.x * scale, bgSize.y * scale);
		//	playVideo.setOrigin(bgSize.x * scale / 2.0, bgSize.y * scale / 2.0);
		//	playVideo.setPosition(winSize.x / 2.0, winSize.y / 2.0);
		//}

		filter.setSize((sf::Vector2f)winSize);
		filter.setFillColor(sf::Color(0, 0, 0, 125));
	}

public:

	Playfield(sf::RenderWindow& window, UI *from, std::unordered_map<std::string, std::string> bmData, sfe::Movie playSong, DB::gameDB gameDB) :
		UI(window, from) , cur(window), beatmapData(bmData), playSong(playSong), gameDB(gameDB)
	{
		std::string base_dir = "D:/osu!/Songs/";
		base_dir += beatmapData["OsuDir"] + "/";

		bmPlay.load(base_dir + beatmapData["OsuFile"]);

		playSong.stop();
		playSong.update();

		if (!playSong.openFromFile(base_dir + bmPlay.General.AudioFilename)) {
			std::cout << "Error open Song" << std::endl;
		}

		
		if (bmPlay.Events.Video == "") {

		} else if (!playVideo.openFromFile(base_dir + bmPlay.Events.Video)) {
			std::cout << "Error open Video" << std::endl;
		} else {
			loadVideo = true;
			playVideo.stop();
		}

		loadHitSound(&bmPlay, base_dir);

		// Calc For Hit Object
		int AR = bmPlay.Difficulty.ApproachRate;

		Beatmap::bmHitObjects::TimePreempt = Beatmap::Difficulty::Range(AR, 1800, 1200, 450);
		Beatmap::bmHitObjects::TimeFadeIn = Beatmap::Difficulty::Range(AR, 1200, 800, 300);

		Beatmap::bmHitObjects::CR = bmPlay.Difficulty.CircleRadius;
		// End Calc For Hit Object
		

		hitwindows.SetDifficulty(bmPlay.Difficulty.OverallDifficulty);
		
		m_window.setKeyRepeatEnabled(false);
		m_window.setMouseCursorGrabbed(true);

		scoreProcessor = new Scoring::ScoreProcessor(&bmPlay);

		font.loadFromFile("resource\\Chakra-Petch-master\\fonts\\ChakraPetch-SemiBoldItalic.ttf");

		
		if (!BackgroundTexture.loadFromFile(base_dir + bmPlay.Events.Background)) {
			Background.setColor(sf::Color(0, 0, 0, 0));
		}

		MutexText.lock();
		textScore.setFont(font);
		textScore.setFillColor(sf::Color::White);
		textCombo.setFont(font);
		textCombo.setFillColor(sf::Color::White);
		textAccuracy.setFont(font);
		textAccuracy.setFillColor(sf::Color::White);

		setScale();
		MutexText.unlock();
	}

	void retry() {
		eventMutext.lock();
		drawMutex.lock();
		updateMutex.lock();

		playSong.stop();

		onDelete();
		
		scoreProcessor = new Scoring::ScoreProcessor(&bmPlay);

		haveStart = 0;
		haveStoreScore = false;

		showHitObj.front = 0;
		showHitObj.back = 0;
		
		eventMutext.unlock();
		drawMutex.unlock();
		updateMutex.unlock();
	}

	virtual ~Playfield() {
		UI::~UI();
		onDelete();
	}
};