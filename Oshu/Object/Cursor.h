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
		loadNewSkin();

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
			this->setPosition((sf::Vector2f) temp);
			oldLocation = temp;
		}
		
	}

	void loadNewSkin() {
		sf::Texture *texture = &Skin::Cursor::cursor;
		sf::Vector2u size = texture->getSize();
		float scale = 60.0 / (float)std::min(size.x, size.y);
		mou.setTexture(*texture);
		mou.setOrigin(size.x / 2.0, size.y / 2.0);
		mou.setScale(scale, scale);
		mou.setScaleFromNow();

		texture = &Skin::Cursor::cursorMiddle;
		size = texture->getSize();
		scale = 20.0 / (float)std::min(size.x, size.y);
		mou2.setTexture(*texture);
		mou2.setOrigin(size.x / 2.0, size.y / 2.0);
		mou2.setScale(scale, scale);
		mou2.setScaleFromNow();
	}

private:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= this->getTransform();
		target.draw(mou, states);
		target.draw(mou2, states);
	}
};

} // namespace Object