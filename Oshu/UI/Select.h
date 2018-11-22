#pragma once

#include <vector>
#include <unordered_map>
#include <cmath>
#include <chrono>
#include <random>

#include <SFML/System.hpp>
#include <sfeMovie/Movie.hpp>

#include "UI.h"
#include "testZone.h"
#include "../DB/beatmap.h"

#include "../Object/Cursor.h"

#include "../Utility/Mutex.h"

class SelectUI : public UI {
	Object::Cursor cur;
	beatmapDB songDB;

	sf::RenderTexture renderText;
	bool updateText = true;
	myMutex updateTextMutex;
	sf::Thread updateTextThread;

	std::vector<std::unordered_map<std::string, std::string>*> *searchData;
	std::string searchKeyword = "";
	std::string oldSearchKeyword = "..";
	bool updateSearch = false;
	float showDataCenter = 0;

	sf::Sprite background;
	sf::Texture backgroundTexture;

	int selectBeatmapSet = -1;
	int selectBeatmapSetId = -1;
	std::vector<std::unordered_map<std::string, std::string>*> *beatmapSetData;
	myMutex beatmapSetDataMutex;
	int selectBeatmapIndex = 0;

	std::default_random_engine generator;

	sfe::Movie playSong;
	myMutex playSongMutex;
	sf::Thread updatePlaySongThread;

	sf::Font font;

protected:
	void OnPressed(sf::Event event) {
		cur.onMouseDown(event.key.code);

		if (event.mouseButton.button == sf::Mouse::Left) {
			if (event.mouseButton.x + 40 >= 500 - (100 * std::cos((event.mouseButton.x - 300) / 325.0))) {
				float newselectBeatmapSet;
				newselectBeatmapSet = ((event.mouseButton.y - 300) / 65.0) + showDataCenter;
				if (selectBeatmapSet != -2 && newselectBeatmapSet > selectBeatmapSet) {
					newselectBeatmapSet -= (*beatmapSetData).size() * 0.6;
					if (newselectBeatmapSet <= selectBeatmapSet + 1) {
						newselectBeatmapSet = selectBeatmapSet;
						selectBeatmapIndex = (((event.mouseButton.y - 300) / 65.0) + showDataCenter - selectBeatmapSet - 1) / 0.6;
						updateText = true;
					}
				}
				selectNewBeatmapSet(newselectBeatmapSet);
			}
		}
	}

	void OnReleased(sf::Event event) {
		cur.onMouseUp(event.key.code);
	}

	void randomSongs() {
		int random;
		if (searchData->size() <= 1) return;
		while ((random = generator() % searchData->size()) == selectBeatmapSet);
		showDataCenter = random + 2;
		selectNewBeatmapSet(random);
		updateText = true;
	}

	void selectNewBeatmapSet(int newBeatmapIndex) {
		if (0 > newBeatmapIndex || newBeatmapIndex >= searchData->size())
			return;
		if (selectBeatmapSet != newBeatmapIndex) {
			selectBeatmapSet = newBeatmapIndex;

			selectBeatmapSetId = stoi((*((*searchData)[newBeatmapIndex]))["id"]);

			beatmapSetDataMutex.lock();
			beatmapSetData = songDB.getBeatmapSet(selectBeatmapSetId);
			beatmapSetDataMutex.unlock();

			updateText = true;
			selectBeatmapIndex = 0;

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

	void updateTextFunc() {
		updateTextMutex.lock();

		this->renderText.setActive(true);

		this->renderText.clear(sf::Color(0, 0, 0, 0));

		sf::Text textTitle;
		sf::Text textDetail;
		sf::Text textSearch;
		
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

		this->renderText.draw(textSearch);

		int from = showDataCenter - 6;
		int to = showDataCenter + 6;

		if (from > selectBeatmapSet) from -= (*beatmapSetData).size() * 0.6;

		if (from < 0) from = 0;
		if (to > (*searchData).size()) to = (*searchData).size();

		for (int i = from; i < to; i++) {
			std::unordered_map<std::string, std::string> row = *((*searchData)[i]);
			float offset = i - showDataCenter;

			if (i > selectBeatmapSet && selectBeatmapSet != -2) {
				offset += (*beatmapSetData).size() * 0.6;
			}

			sf::Vector2f position;
			position.x = 500 - (100 * std::cos(offset / 5.0));
			position.y = 300 + (offset * 65.0);

			if (i == selectBeatmapSet) {
				sf::Text textVersion;
				textVersion.setFont(font);
				textVersion.setCharacterSize(12);
				textVersion.setFillColor(sf::Color::White);
				for (int j = 0; j < (*beatmapSetData).size(); j++) {
					float offset = i - showDataCenter + j * 0.6 + 1;
					sf::Vector2f position;
					position.x = 500 - (100 * std::cos(offset / 5.0)) - 20;
					position.y = 300 + (offset * 65.0);

					if (j == selectBeatmapIndex) {
						position.x -= 40;
					}

					textVersion.setString((*((*beatmapSetData)[j]))["Version"]);
					textVersion.setPosition(position);

					this->renderText.draw(textVersion);
				}
				position.x -= 80;
			}
				

			textTitle.setString(row["Title"]);
			textTitle.setPosition(position);

			textDetail.setString(row["Artist"] + " // " + row["Creator"]);
			textDetail.setPosition(position + sf::Vector2f(0, 20));
			
			this->renderText.draw(textTitle);
			this->renderText.draw(textDetail);
		}
		
		this->renderText.display();
		updateText = false;

		//this->renderText.setActive(false);

		updateTextMutex.unlock();
	}

	void updatePlaySong() {
		playSongMutex.lock();
		if (beatmapSetDataMutex.tryLock()) {
			int time = stoi((*((*beatmapSetData)[0]))["PreviewTime"]);
			beatmapSetDataMutex.unlock();

			// some song block by seek and some song can not seek
			if (playSong.getDuration() > sf::milliseconds(time) && time > 0) {
				if (!playSong.setPlayingOffset(sf::milliseconds(time))) {
					playSong.setPlayingOffset(sf::milliseconds(0));
				}
			}
			if (isUIshow())
				playSong.play();
		}
		playSongMutex.unlock();
	}

	virtual void gotoUI(UI *ui) {
		UI::gotoUI(ui);

		playSong.stop();
	}

	virtual void onComeBack() {
		UI::onComeBack();
		m_window.setKeyRepeatEnabled(true);
		m_window.setMouseCursorGrabbed(false);
		//playSong.play();
	}

	virtual void onUpdate() {
		if (!isUIshow())
			return;

		cur.update();

		if (updateSearch) {
			if (oldSearchKeyword != searchKeyword) {
				updateSearch = false;
				oldSearchKeyword = searchKeyword;

				sf::Clock aa;
				std::cout << "Search : " << searchKeyword << std::endl;
				searchData = songDB.search(searchKeyword);
				std::cout << "Use time : " << aa.getElapsedTime().asMilliseconds() << " ms." << std::endl;

				if (selectBeatmapSet == -1) {
					randomSongs();
				}
				else {
					if ((*searchData).size() == 1 && searchKeyword.size() > 6) {
						selectNewBeatmapSet(0);
					}
					else {
						int i = 0;
						for (std::unordered_map<std::string, std::string>* row : *searchData) {
							if (std::stoi((*row)["id"]) == selectBeatmapSetId) {
								showDataCenter = i - (selectBeatmapSet != -2 ? (selectBeatmapSet - showDataCenter) : 0);
								selectBeatmapSet = i;
								break;
							}
							i++;
						}

						if (i == (*searchData).size()) {
							std::cout << "Not Found" << std::endl;
							selectBeatmapSet = -2;
						}
					}
				}

				updateText = true;
			}
		}

		if (showDataCenter < 0) showDataCenter = 0;
		else {
			float limit = (*searchData).size() - 1;
			if (selectBeatmapSet != -2)
				limit += (*beatmapSetData).size() * 0.6;
			if (showDataCenter > limit)
				showDataCenter = limit;
		}

		if (updateTextMutex.tryLock()) {
			updateTextMutex.unlock();
			if (updateText) {
				updateTextFunc();
				//updateTextThread.launch();
			}
		}


		if (playSongMutex.tryLock()) {
			playSongMutex.unlock();
			if (playSong.getStatus() == sfe::Status::Stopped) {
				//updatePlaySong();
				updatePlaySongThread.launch();
			}
			playSong.update();
		}
	}

	void onDraw() {
		m_window.draw(background);

		const sf::Texture& texture = renderText.getTexture();
		sf::Sprite sprite(texture);
		m_window.draw(sprite);

		m_window.draw(cur);
	}

	void onEvent(sf::Event event) {
		switch (event.type) {
		case sf::Event::MouseButtonPressed:
			OnPressed(event);
			break;

		case sf::Event::MouseButtonReleased:
			OnReleased(event);
			break;

		case sf::Event::MouseWheelMoved:
			updateTextMutex.lock();
			showDataCenter -= event.mouseWheel.delta / 2.5;
			updateText = true;
			updateTextMutex.unlock();
			break;

		case sf::Event::KeyPressed:
			switch (event.key.code) {
			case sf::Keyboard::Escape:
				if (searchKeyword != "") searchKeyword = "";
				break;
			case sf::Keyboard::F2:
				randomSongs();
				break;
			case sf::Keyboard::Enter:
				gotoUI(new testUI(m_window, this, *((*beatmapSetData)[selectBeatmapIndex]), &playSong));
				break;

			case sf::Keyboard::F5:
				std::cout << "Start Update DB" << std::endl;
				songDB.update();
				std::cout << "Finish" << std::endl;
				break;

			case sf::Keyboard::Left:
				selectNewBeatmapSet(selectBeatmapSet - 1);
				showDataCenter = selectBeatmapSet;
				updateText = true;
				break;
			case sf::Keyboard::Right:
				selectNewBeatmapSet(selectBeatmapSet + 1);
				showDataCenter = selectBeatmapSet;
				updateText = true;
				break;

			case sf::Keyboard::Up:
				if (selectBeatmapIndex == 0) {
					if (selectBeatmapSet != 0) {
						selectNewBeatmapSet(selectBeatmapSet - 1);
						showDataCenter = selectBeatmapSet;
						selectBeatmapIndex = (*beatmapSetData).size() - 1;
						std::cout << " -- " << selectBeatmapIndex << std::endl;
					}
				}
				else {
					selectBeatmapIndex--;
				}
				updateText = true;
				break;
			case sf::Keyboard::Down:
				if (selectBeatmapIndex == (*beatmapSetData).size() - 1) {
					if (selectBeatmapSet != (*searchData).size() - 1) {
						selectNewBeatmapSet(selectBeatmapSet + 1);
						showDataCenter = selectBeatmapSet;
					}
				}
				else {
					selectBeatmapIndex++;
				}
				updateText = true;
				break;

			case sf::Keyboard::PageUp:
				showDataCenter -= 8;
				updateText = true;
				break;
			case sf::Keyboard::PageDown:
				showDataCenter += 8;
				updateText = true;
				break;
			}
			break;

		case sf::Event::TextEntered:
			if (event.text.unicode == '\b') {
				if (searchKeyword.size() > 0)
					searchKeyword.pop_back();
			}
			else if (32 <= event.text.unicode && event.text.unicode <= 126) {
				searchKeyword.push_back(event.text.unicode);
			}
			updateSearch = true;
			break;
		}
	}

public:
	SelectUI(sf::RenderWindow& window, UI *from, beatmapDB DB) : UI(window, from), cur(window), songDB(DB),
		updateTextThread(&SelectUI::updateTextFunc, this),
		updatePlaySongThread(&SelectUI::updatePlaySong, this)
	{
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

		renderText.setActive(false);

		font.loadFromFile("resource\\Chakra-Petch-master\\fonts\\ChakraPetch-SemiBoldItalic.ttf");
	}
};