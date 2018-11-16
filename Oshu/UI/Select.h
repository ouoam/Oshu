#pragma once

#include <vector>
#include <unordered_map>

#include "UI.h"
#include "../DB/beatmap.h"

#include "../Object/Cursor.h"

class SelectUI : public UI {
	Object::Cursor cur;
	beatmapDB songDB;

	int startShowData = 0;

	sf::RenderTexture renderText;
	bool updateText = true;
	sf::Mutex updateTextMutex;

	std::vector<std::unordered_map<std::string, std::string>*> *searchData;
	std::string searchKeyword = "";
	std::string oldSearchKeyword = "..";
	bool updateSearch = false;

protected:
	void OnPressed(sf::Event event) {
		cur.onMouseDown(event.key.code);
	}

	void OnReleased(sf::Event event) {
		cur.onMouseUp(event.key.code);
	}

public:
	SelectUI(sf::RenderWindow& window, beatmapDB DB) : UI(window), cur(window) , songDB(DB) {
		//test Memory leak
		//for (int i = 0; i < 500; i++) {
		//	songDB.search(searchKeyword);
		//	std::cout << i << std::endl;
		//}
		updateSearch = true;
		if (!renderText.create(800, 600)) {
			std::cout << "Error create render text" << std::endl;
		}
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
				if (startShowData > (*searchData).size() - 1) startShowData = (*searchData).size() - 1;

				
			}
		}

		updateTextMutex.lock();
		if (updateText) {
			renderText.clear();

			sf::Text textTitle;
			sf::Text textDetail;
			sf::Font font;
			font.loadFromFile("arial.ttf");

			textTitle.setFont(font);
			textTitle.setCharacterSize(16);
			textTitle.setFillColor(sf::Color::White);

			textDetail.setFont(font);
			textDetail.setCharacterSize(12);
			textDetail.setFillColor(sf::Color::White);

			int to = (*searchData).size();
			if (to - startShowData > 8) to = startShowData + 8;

			for (int i = startShowData; i < to; i++) {
				std::unordered_map<std::string, std::string> row = *((*searchData)[i]);

				textTitle.setString(row["Title"]);
				textTitle.setPosition(10, 100 + (i - startShowData) * 60);

				textDetail.setString(row["Artist"] + " // " + row["Creator"]);
				textDetail.setPosition(10, 120 + (i - startShowData) * 60);

				renderText.draw(textTitle);
				renderText.draw(textDetail);
			}

			renderText.display();
			updateText = false;
		}
		updateTextMutex.unlock();
		
	}

	void draw() {
		const sf::Texture& texture = renderText.getTexture();
		sf::Sprite sprite(texture);
		m_window.draw(sprite);

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

		case sf::Event::MouseWheelMoved:
			updateTextMutex.lock();
			startShowData -= event.mouseWheel.delta;
			if (startShowData < 0) startShowData = 0;
			else if (startShowData > (*searchData).size() - 1) startShowData = (*searchData).size() - 1;
			updateText = true;
			updateTextMutex.unlock();
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