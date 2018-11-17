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
#include "DB/beatmap.h"

#include "UI/UI.h"

//#include "UI/Playfield.h"
#include "UI/testZone.h"
#include "UI/Select.h"

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
	//std::cout << "Start Update Beatmap Database" << std::endl;
	//aa.update();
	//std::cout << "Finish Update" << std::endl;
	
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(800, 600), "Oshu!", sf::Style::Titlebar | sf::Style::Close, settings);
	//window.setMouseCursorVisible(false);
	//window.setFramerateLimit(240);
	//window.setVerticalSyncEnabled(true);
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
				window.setFramerateLimit(60);
				break;

			case sf::Event::GainedFocus:
				window.setFramerateLimit(240);
				break;
			}

			ui->onEvent(event);
		}
	}

	return 0;
}
