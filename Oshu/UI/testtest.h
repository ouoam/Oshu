#pragma once

#include <iostream>
#include <deque>
#include <cmath>
#include <windows.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "../Object/Cursor.h"

#include "UI.h"


class testtest : public UI {

	Object::Cursor cur;

	sf::Image image;
	sf::Texture texture;
	sf::Sprite sprite;

	sf::Transform tranform;

protected:
	void OnPressed(sf::Event event) {
		cur.onMouseDown(event.key.code);
	}

	void OnReleased(sf::Event event) {
		cur.onMouseUp(event.key.code);
	}

public:

	testtest(sf::RenderWindow& window, UI *from) :
		UI(window, from), cur(window) {

		if (!image.loadFromFile("resource\\Osu!Logo_(2015).png")) {
			std::cout << "can not load texture" << std::endl;
			system("pause");
		}

		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sf::Vector2u size = texture.getSize();
		sprite.setOrigin(size.x / 2.0, size.y / 2.0);
		sprite.setScale(0.1, 0.1);

		tranform.scale(2, 2, 400, 300);
	}

	virtual ~testtest() {
		UI::~UI();
	}


	void onUpdate() {
		cur.update();
		sprite.setPosition(cur.getPosition());
	}

	void onDraw() {

		m_window.draw(sprite, tranform.getInverse());

		m_window.draw(cur);
		
		
	}

	void onEvent(sf::Event event) {
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