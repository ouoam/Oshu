#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class UI {
protected:
	sf::RenderWindow& m_window;

public:
	UI(sf::RenderWindow& window) : m_window(window) {}

	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void newEvent(sf::Event event) = 0;

};