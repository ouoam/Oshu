#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class UI {
private:
	UI *fromUI = nullptr;
	UI *toUI = nullptr;

	bool comebackUI = false;

protected:
	sf::RenderWindow& m_window;

	virtual inline void gotoUI(UI *ui) {
		toUI = ui;
	}

	virtual inline void gobackUI() {
		if (fromUI != nullptr) {
			fromUI->comebackUI = true;
			toUI = fromUI;
		}
	}

	inline bool isUIshow() {
		return toUI == nullptr;
	}

public:
	UI(sf::RenderWindow& window, UI *from) : m_window(window) , fromUI(from) {}

	virtual ~UI() {}

	virtual inline void update() {
		if (comebackUI) {
			delete toUI;
			toUI = nullptr;
			comebackUI = false;
			onComeBack();
		}
	}
	virtual void draw() = 0;
	virtual void onEvent(sf::Event event) = 0;
	virtual void onComeBack() {};

	inline UI *nowUI() {
		if (toUI != nullptr)
			return toUI;
		return this;
	}
};