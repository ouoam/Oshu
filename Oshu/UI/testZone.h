#pragma once

#include <iostream>
#include <vector>

#include <SFML/System.hpp>

#include "../Object/Container.h"
#include "../Object/Cursor.h"
#include "../Object/Circle.h"


namespace UI {

Object::Cursor *cur;

std::vector<Object::Container*> objs;

sf::Mutex Mutex;

struct A {
	sf::RenderWindow& m_window;
	A(sf::RenderWindow& window) : m_window(window) {}
}*win;

void load(sf::RenderWindow& window) {
	win = new A(window);
	cur = new Object::Cursor(window);
}

void OnPressed(sf::Event event) {
	cur->onMouseDown(event.key.code);

	sf::Vector2i temp = sf::Mouse::getPosition(win->m_window);
	
	Object::Circle *newCircle = new Object::Circle();
	newCircle->setPosition(sf::Vector2f(temp) + sf::Vector2f(-60, 0));
	newCircle->StartPreemptState();

	Mutex.lock();
	objs.push_back(newCircle);
	Mutex.unlock();
}

void OnReleased(sf::Event event) {
	cur->onMouseUp(event.key.code);
}

void update() {
	cur->update();

	Mutex.lock();
	std::vector<Object::Container*>::iterator it = objs.begin();
	while (it != objs.end()) {
		(**it).update();

		if ((**it).willBeRemove) {
			delete *it;  // Because I use new for Object::Circle

			it = objs.erase(it);
			if (it != objs.begin()) {
				it = std::prev(it);
			}
		}
		else {
			++it;
		}
	}
	Mutex.unlock();
}

void draw() {
	win->m_window.draw(*cur);

	Mutex.lock();
	for (auto obj : objs) {
		win->m_window.draw(*obj);
	}
	Mutex.unlock();

}

}