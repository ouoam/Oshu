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
	virtual void onDelete() {
		eventMutext.lock();
		drawMutex.lock();
		updateMutex.lock();
	};
	virtual void onComeBack() {};

public:
	UI(sf::RenderWindow& window, UI *from) : m_window(window) , fromUI(from) {}

	//virtual ~UI() {
	//	onDelete();
	//}

	void update() {
		if (updateMutex.tryLock()) {
			if (comebackUI) {
				toUI->onDelete();
				delete toUI;
				toUI = nullptr;
				comebackUI = false;
				onComeBack();
			}
			onUpdate();
			updateMutex.unlock();
		}
	}
	void draw() {
		if (drawMutex.tryLock()) {
			onDraw();
			drawMutex.unlock();
		}
	}

	void newEvent(sf::Event event) {
		if (eventMutext.tryLock()) {
			onEvent(event);
			eventMutext.unlock();
		}
	}

	inline UI *nowUI() {
		if (toUI != nullptr)
			return toUI;
		return this;
	}
};