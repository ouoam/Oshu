#include <string>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Skin/Skin.h"
#include "DB/gameDB.h"

#include "UI/UI.h"
#include "UI/Select.h"
#include "UI/testtest.h"

UI *ui;

void renderThread(sf::RenderWindow* window)
{
	while (window->isOpen())
	{
		window->setActive(true);

		window->clear();

		ui->draw();

		window->display();
	}
}

void updateThread(sf::RenderWindow* window) {
	sf::Time m_frameTimeLimit = sf::seconds(1.f / 120);
	sf::Clock m_clock;
	while (window->isOpen()) {
		ui = ui->nowUI();

		ui->update();

		sf::sleep(m_frameTimeLimit - m_clock.getElapsedTime());
		m_clock.restart();
	}
}


int main()
{
	DB::gameDB aa;
	
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(800, 600), "Oshu!", sf::Style::Default, settings);
	//sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Oshu!", sf::Style::Fullscreen, settings);
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);

	Skin::load();
	ui = new SelectUI(window, nullptr, aa);
	//ui = new testtest(window, nullptr);

	window.setActive(false);
	sf::Thread thread(&renderThread, &window);
	sf::Thread thread2(&updateThread, &window);
	thread.launch();
	thread2.launch();

	bool fullscreen = false;

	aa.setUser("test user");

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
				window.setFramerateLimit(60);
				break;

			case sf::Event::Resized:
				window.setView(sf::View(sf::FloatRect(0, 0, (float)window.getSize().x, (float)window.getSize().y)));
				break;
			//case sf::Event::KeyPressed:
			//	switch (event.key.code) {
			//	case sf::Keyboard::F11:
			//		fullscreen = !fullscreen;

			//		if (fullscreen)
			//			window.create(sf::VideoMode::getDesktopMode(), "Oshu!", sf::Style::Fullscreen, settings);
			//		else
			//			window.create(sf::VideoMode(800, 600), "Oshu!", sf::Style::Default, settings);

			//		window.setFramerateLimit(60);
			//		window.setActive(false);
			//	}
			}

			ui->newEvent(event);
		}
	}

	return 0;
}
