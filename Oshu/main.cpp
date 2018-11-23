#include <string>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Skin/Skin.h"
#include "DB/beatmap.h"

#include "UI/UI.h"
#include "UI/Select.h"
#include "UI/testtest.h"

UI *ui;

void renderingThread(sf::RenderWindow* window)
{
	window->setActive(true);

	while (window->isOpen())
	{
		ui = ui->nowUI();

		ui->update();

		window->clear();

		ui->draw();

		window->display();
	}
}


int main()
{
	beatmapDB aa;
	
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(800, 600), "Oshu!", sf::Style::Titlebar | sf::Style::Close, settings);
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);

	Skin::load();
	ui = new SelectUI(window, nullptr, aa);

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

			case sf::Event::MouseEntered:
				window.setMouseCursorVisible(false);
				break;

			case sf::Event::MouseLeft:
				window.setMouseCursorVisible(true);
				break;

			case sf::Event::LostFocus:
				window.setFramerateLimit(30);
				break;

			case sf::Event::GainedFocus:
				window.setFramerateLimit(120);
				break;
			}

			ui->newEvent(event);
		}
	}

	return 0;
}
