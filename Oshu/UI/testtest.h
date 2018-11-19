#pragma once

#include <iostream>
#include <deque>
#include <cmath>
#include <windows.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "../Object/Cursor.h"

#include "UI.h"

namespace {
bool setShape(HWND hWnd, const sf::Image& image) {
	const sf::Uint8* pixelData = image.getPixelsPtr();
	HRGN hRegion = CreateRectRgn(0, 0, image.getSize().x, image.getSize().y);

	// Determine the visible region
	for (unsigned int y = 0; y < image.getSize().y; y++) {
		for (unsigned int x = 0; x < image.getSize().x; x++) {
			if (pixelData[y * image.getSize().x * 4 + x * 4 + 3] <= 100) {
				HRGN hRegionPixel = CreateRectRgn(x, y, x + 1, y + 1);
				CombineRgn(hRegion, hRegion, hRegionPixel, RGN_XOR);
				DeleteObject(hRegionPixel);
			}
		}
	}

	SetWindowRgn(hWnd, hRegion, true);
	DeleteObject(hRegion);
	return true;
}

bool setTransparency(HWND hWnd, unsigned char alpha) {
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
	return true;
}
}

class testtest : public UI {

	Object::Cursor cur;

	sf::Image image;
	sf::Texture texture;
	sf::Sprite sprite;

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

		setShape(window.getSystemHandle(), image);
		setTransparency(window.getSystemHandle(), 255);

		texture.loadFromImage(image);
		sprite.setTexture(texture);
		//sprite.setScale(0.5, 0.5);
	}

	virtual ~testtest() {
		UI::~UI();
	}


	void update() {
		cur.update();
	}

	void draw() {
		m_window.draw(sprite);
		//m_window.draw(cur);
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