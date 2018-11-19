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

	sf::Shader shader;

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
		//sprite.setScale(0.5, 0.5);


		if (!shader.loadFromFile("blur.frag", sf::Shader::Fragment)) {
			std::cout << "Error GLSL" << std::endl;
		}

		shader.setUniform("texture", sf::Shader::CurrentTexture);
	}

	virtual ~testtest() {
		UI::~UI();
	}


	void update() {
		cur.update();

		shader.setUniform("alpha", 0.7f);
	}

	void draw() {
		m_window.draw(cur);

		sf::BlendMode blendmode;
		blendmode.colorSrcFactor = sf::BlendMode::SrcAlpha;
		blendmode.colorDstFactor = sf::BlendMode::OneMinusSrcAlpha;
		blendmode.colorEquation = sf::BlendMode::Add;
		sf::RenderStates states;
		states.shader = &shader;
		states.texture = sprite.getTexture();
		states.blendMode = blendmode;
		m_window.draw(sprite, states);
		
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