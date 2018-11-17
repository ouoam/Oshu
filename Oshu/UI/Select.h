#pragma once

#include <vector>
#include <unordered_map>
#include <cmath>
#include <chrono>
#include <random>

#include <sfeMovie/Movie.hpp>

#include "UI.h"
#include "../DB/beatmap.h"

#include "../Object/Cursor.h"

class SelectUI : public UI {
	Object::Cursor cur;
	beatmapDB songDB;

	sf::RenderTexture renderText;
	bool updateText = true;
	sf::Mutex updateTextMutex;

	std::vector<std::unordered_map<std::string, std::string>*> *searchData;
	std::string searchKeyword = "";
	std::string oldSearchKeyword = "..";
	bool updateSearch = false;
	float showDataCenter = 0;

	sf::Sprite background;
	sf::Texture backgroundTexture;

	int selectSong = -1;
	std::vector<std::unordered_map<std::string, std::string>*> *beatmapSetData;

	std::default_random_engine generator;

	sfe::Movie playSong;
	sf::Mutex playSongMutex;

protected:
	void OnPressed(sf::Event event) {
		cur.onMouseDown(event.key.code);
	}

	void OnReleased(sf::Event event) {
		cur.onMouseUp(event.key.code);
	}

	void randomSongs() {
		int random;
		while ((random = generator() % searchData->size()) == selectSong);
		showDataCenter = random;
		selectNewSongs(random);
		updateText = true;
	}

	void selectNewSongs(int newSong) {
		if (selectSong != newSong) {
			selectSong = newSong;
			updateText = true;

			int beatmapID = stoi((*((*searchData)[newSong]))["id"]);

			beatmapSetData = songDB.getBeatmapSet(beatmapID);

			for (auto v : *beatmapSetData) {
				std::cout << (*v)["OsuFile"] << std::endl;
			}

			std::string path = "D:/osu!/Songs/";
			path += (*((*beatmapSetData)[0]))["OsuDir"] + "/";
			path += (*((*beatmapSetData)[0]))["AudioFilename"];

			playSongMutex.lock();
			if (!playSong.openFromFile(path)) {
				std::cout << "Error open media" << std::endl;
				playSongMutex.unlock();
				return;
			}
			playSongMutex.unlock();
		}
		
	}

public:
	SelectUI(sf::RenderWindow& window, beatmapDB DB) : UI(window), cur(window) , songDB(DB) {
		updateSearch = true;
		if (!renderText.create(800, 600)) {
			std::cout << "Error create render text" << std::endl;
		}

		m_window.setKeyRepeatEnabled(true);

		if (!backgroundTexture.loadFromFile("resource\\osu-resources-master\\osu.Game.Resources\\Textures\\Backgrounds\\bg2.jpg")) {
			std::cout << "55" << std::endl;
		}
		background.setTexture(backgroundTexture);
		
		sf::Vector2u winSize = m_window.getSize();
		sf::Vector2u bgSize = backgroundTexture.getSize();
		double sx = (double)winSize.x / (double)bgSize.x;
		double sy = (double)winSize.y / (double)bgSize.y;
		
		background.setScale(sf::Vector2f(std::max(sx,sy), std::max(sx, sy)));

		background.setOrigin(bgSize.x / 2.0, bgSize.y / 2.0);
		background.setPosition(winSize.x / 2.0, winSize.y / 2.0);

		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		generator.seed(seed);
	}

	void update() {
		cur.update();

		if (updateSearch) {
			if (oldSearchKeyword != searchKeyword) {
				updateSearch = false;
				oldSearchKeyword = searchKeyword;

				sf::Clock aa;
				std::cout << "Search : " << searchKeyword << std::endl;
				searchData = songDB.search(searchKeyword);
				std::cout << "Use time : " << aa.getElapsedTime().asMilliseconds() << " ms." << std::endl;

				updateText = true;
				if (showDataCenter > (*searchData).size() - 1) showDataCenter = (*searchData).size() - 1;
			}
		}

		if (selectSong == -1) {
			randomSongs();
		}

		updateTextMutex.lock();
		if (updateText) {
			renderText.clear(sf::Color(0,0,0,0));

			sf::Text textTitle;
			sf::Text textDetail;
			sf::Text textSearch;
			sf::Font font;
			font.loadFromFile("resource\\Chakra-Petch-master\\fonts\\ChakraPetch-SemiBoldItalic.ttf");

			textTitle.setFont(font);
			textTitle.setCharacterSize(16);
			textTitle.setFillColor(sf::Color::White);

			textDetail.setFont(font);
			textDetail.setCharacterSize(12);
			textDetail.setFillColor(sf::Color::White);

			textSearch.setFont(font);
			textSearch.setCharacterSize(16);
			textSearch.setFillColor(sf::Color::White);
			textSearch.setString(searchKeyword);
			textSearch.setPosition(450, 40);

			renderText.draw(textSearch);

			int from = showDataCenter - 6;
			int to = showDataCenter + 6;

			if (from < 0) from = 0;
			if (to > (*searchData).size()) to = (*searchData).size();

			for (int i = from; i < to; i++) {
				std::unordered_map<std::string, std::string> row = *((*searchData)[i]);
				float offset = i - showDataCenter;

				sf::Vector2f position;
				position.x = 500 - (100 * std::cos(offset / 5.0));
				position.y = 300 + (offset * 65.0);

				if (i == selectSong)
					position.x -= 80;

				textTitle.setString(row["Title"]);
				textTitle.setPosition(position);

				textDetail.setString(row["Artist"] + " // " + row["Creator"]);
				textDetail.setPosition(position + sf::Vector2f(0, 20));

				renderText.draw(textTitle);
				renderText.draw(textDetail);
			}

			renderText.display();
			updateText = false;
		}
		updateTextMutex.unlock();
		
		playSongMutex.lock();
		if (playSong.getStatus() == sfe::Status::Stopped) {
			int time = stoi((*((*beatmapSetData)[0]))["PreviewTime"]);
			//if (time < 0) time = 0;
			if (playSong.getDuration() > sf::milliseconds(time)) {
				if (!playSong.setPlayingOffset(sf::milliseconds(time))) {
					playSong.setPlayingOffset(sf::milliseconds(0));
				}
			}
			playSong.play();
		}
		playSong.update();
		playSongMutex.unlock();
	}

	void draw() {
		m_window.draw(background);

		const sf::Texture& texture = renderText.getTexture();
		sf::Sprite sprite(texture);
		m_window.draw(sprite);


		m_window.draw(cur);
	}

	void newEvent(sf::Event event) {
		int newSelectSong;
		switch (event.type) {
		case sf::Event::MouseButtonPressed:
			OnPressed(event);
			if (event.mouseButton.button == sf::Mouse::Left) {
				newSelectSong = ((event.mouseButton.y - 300) / 65.0) + showDataCenter;
				if (0 <= newSelectSong && newSelectSong < searchData->size())
					selectNewSongs(newSelectSong);
			}
			break;

		case sf::Event::MouseButtonReleased:
			OnReleased(event);
			break;

		case sf::Event::MouseWheelMoved:
			updateTextMutex.lock();
			showDataCenter -= event.mouseWheel.delta / 2.5;
			if (showDataCenter < 0) showDataCenter = 0;
			else if (showDataCenter > (*searchData).size() - 1) showDataCenter = (*searchData).size() - 1;
			updateText = true;
			updateTextMutex.unlock();
			break;

		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Escape) {
				if (searchKeyword != "") searchKeyword = "";
			}
			else if (event.key.code == sf::Keyboard::F2) {
				randomSongs();
			}
			break;

		case sf::Event::TextEntered:
			if (event.text.unicode == '\b') {
				if (searchKeyword.size() > 0)
					searchKeyword.pop_back();
			} else if (32 <= event.text.unicode && event.text.unicode <= 126) {
				searchKeyword.push_back(event.text.unicode);
			}
			updateSearch = true;
			break;
		}
	}
};