#pragma once

#include <vector>
#include <unordered_map>

#include "UI.h"
#include "../DB/beatmap.h"

#include "../Object/Cursor.h"

class SelectUI : public UI {
	Object::Cursor cur;
	beatmapDB songDB;

	std::vector<std::unordered_map<std::string, std::string>> *searchData;

protected:
	void OnPressed(sf::Event event) {
		cur.onMouseDown(event.key.code);
	}

	void OnReleased(sf::Event event) {
		cur.onMouseUp(event.key.code);
	}

public:
	SelectUI(sf::RenderWindow& window, beatmapDB DB) : UI(window), cur(window) , songDB(DB){
		searchData = songDB.search("accel");

		for (std::unordered_map<std::string, std::string> row : *searchData) {
			for (std::pair<const std::string, std::string> field : row) {
				std::cout << field.second << "\t";
			}
			std::cout << std::endl;
		}
	}

	void update() {
		cur.update();
	}

	void draw() {
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