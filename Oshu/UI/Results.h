#pragma once

#include "UI.h"
#include "../DB/gameDB.h"
#include "../Object/Cursor.h"
#include "../Skin/Skin.h"

#include "../Scoring/ScoreProcessor.h"

class Results : public UI {
	DB::gameDB gameDB;
	Object::Cursor cur;

	Scoring::Score record;

	sf::Sprite rankingPanel;

	void onDraw() {
		m_window.draw(rankingPanel);

		m_window.draw(cur);
	}

	void onUpdate() {
		cur.update();
	}

	void setScale() {
		sf::Vector2u winSize = m_window.getSize();
	}

public:
	Results(sf::RenderWindow& window, UI *from, DB::gameDB DB, Scoring::Score rec) :
		UI(window, from), cur(window), gameDB(DB), record(rec)
	{
		rankingPanel.setTexture(*Skin::get("ranking-panel"));

		setScale();
	}
};