#include <Time.h>
#include <string>
#include <fstream>
//#include <deque>
#include <cmath>
#include <sys/stat.h>

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
//#include <SFML/Audio.hpp>

//#include "Beatmap/Beatmap.h"
//#include "Audio/hitsound.h"

#include "Object/Animate/Animate.h"
#include "Skin/Skin.h"

#include "UI/Playfield.h"


void renderingThread(sf::RenderWindow* window)
{
	window->setActive(true);

	while (window->isOpen())
	{
		UI::update();

		//window->clear(sf::Color::Black);
		window->clear();

		UI::draw();

		window->display();
	}
}



int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!", sf::Style::Titlebar | sf::Style::Close, settings);
	window.setMouseCursorVisible(false);
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);

	Skin::load();
	UI::load(window);

	window.setActive(false);
	sf::Thread thread(&renderingThread, &window);
	thread.launch();

	while (window.isOpen())
	{
		sf::Event event;
		if (window.waitEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::KeyPressed:
			case sf::Event::MouseButtonPressed:
				UI::OnPressed(event);
				break;

			case sf::Event::KeyReleased:
			case sf::Event::MouseButtonReleased:
				UI::OnReleased(event);
				break;

			case sf::Event::MouseEntered:
				window.setMouseCursorVisible(false);
				break;

			case sf::Event::MouseLeft:
				window.setMouseCursorVisible(true);
				break;
			}
		}
	}

	return 0;
}
