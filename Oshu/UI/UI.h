#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "../Utility/Mutex.h"

class UI {
private:
	UI *fromUI = nullptr;
	UI *toUI = nullptr;

	bool comebackUI = false;

protected:
	sf::RenderWindow& m_window;
	myMutex updateMutex;
	myMutex drawMutex;
	myMutex eventMutext;

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

	virtual void onUpdate() {};
	virtual void onDraw() {};
	virtual void onEvent(sf::Event event) {};
	virtual void onDelete() {};
	virtual void onComeBack() {};

public:
	UI(sf::RenderWindow& window, UI *from) : m_window(window) , fromUI(from) {}

	virtual ~UI() {
		//eventMutext.lock();
		//drawMutex.lock();
		//updateMutex.lock();

		onDelete();
	}

	void update() {
		updateMutex.lock();
		if (comebackUI) {
			delete toUI;
			toUI = nullptr;
			comebackUI = false;
			onComeBack();
		}
		onUpdate();
		updateMutex.unlock();
	}
	void draw() {
		drawMutex.lock();
		onDraw();
		drawMutex.unlock();
	}

	void newEvent(sf::Event event) {
		eventMutext.lock();
		onEvent(event);
		eventMutext.unlock();
	}

	inline UI *nowUI() {
		if (toUI != nullptr)
			return toUI;
		return this;
	}
};