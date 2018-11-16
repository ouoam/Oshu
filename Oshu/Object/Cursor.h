#pragma once

#include <SFML/Graphics.hpp>

#include "Container.h"
#include "Animate/Animate.h"
#include "../Skin/Skin.h"

namespace Object {

class Cursor : public Container {
	Animate::AnimeSprite mou;
	Animate::AnimeSprite mou2;

	int downCount = 0;

	sf::RenderWindow& m_window;

	sf::Clock clock;
	//sf::Clock clock2;
	sf::Vector2i oldLocation;

public:

	Cursor(sf::RenderWindow& window) : m_window(window) {
		sf::Vector2u size = Skin::Cursor::cursor.getSize();
		mou.setTexture(Skin::Cursor::cursor);
		mou.setOrigin(size.x / 2.0, size.y / 2.0);
		mou.scale(sf::Vector2f(0.4, 0.4));
		mou.setScaleFromNow();

		sf::Vector2u size2 = Skin::Cursor::cursorMiddle.getSize();
		mou2.setTexture(Skin::Cursor::cursorMiddle);
		mou2.setOrigin(size2.x / 2.0, size2.y / 2.0);
		mou2.scale(sf::Vector2f(0.3, 0.3));

		downCount = 0;
	}

	virtual void onMouseDown(uint8_t key) {
		switch (key) {
		//case sf::Mouse::Left:
		//case sf::Mouse::Right:
		case sf::Keyboard::S:
		case sf::Keyboard::A:
			downCount++;
			mou.scaleTo(1.0).scaleTo(1.3, 100, Object::Animate::OutQuad);
			break;
		}
			
	}

	virtual void onMouseUp(uint8_t key) {
		switch (key) {
		//case sf::Mouse::Left:
		//case sf::Mouse::Right:
		case sf::Keyboard::S:
		case sf::Keyboard::A:
			if (--downCount == 0)
				mou.scaleTo(1.0, 200, Object::Animate::OutQuad);
		}
	}

	

	void update() {
		mou.update();
		mou.setRotation(clock.getElapsedTime().asSeconds() / 10.0 * 360.0);

		sf::Vector2i temp = sf::Mouse::getPosition(m_window);

		if (temp != oldLocation) {
			//clock2.restart();
			mou.setPosition((sf::Vector2f) temp);
			mou2.setPosition((sf::Vector2f) temp);
			oldLocation = temp;
		}
		
	}

private:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(mou, states);
		target.draw(mou2, states);
	}
};

} // namespace Object