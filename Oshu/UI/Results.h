#pragma once

#include <SFML/Graphics.hpp>

#include "UI.h"
#include "../DB/gameDB.h"
#include "../Object/Cursor.h"
#include "../Skin/Skin.h"
#include "../Beatmap/Beatmap.h"

#include "../Scoring/ScoreProcessor.h"
#include "../Utility/stl.h"

class Results : public UI {
	DB::gameDB gameDB;
	std::unordered_map<std::string, std::string> beatmapData;
	Scoring::Score record;

	Beatmap::Beatmap bmPlay;

	Object::Cursor cur;

	sf::Font font;
	sf::Text Line1;
	sf::Text Line2;
	sf::Text Line3;

	sf::Sprite rankingPanel;
	sf::Sprite bg;
	sf::Sprite rankingTitle;
	sf::Sprite ranking;

	sf::Texture bgTexture;

	sf::RectangleShape topFilter;

	void onDraw() {
		m_window.draw(bg);
		m_window.draw(topFilter);

		m_window.draw(rankingPanel);

		m_window.draw(Line1);
		m_window.draw(Line2);
		m_window.draw(Line3);

		m_window.draw(ranking);
		m_window.draw(rankingTitle);

		m_window.draw(cur);
	}

	void onUpdate() {
		cur.update();
	}

	void onEvent(sf::Event event) {
		switch (event.type) {
		case sf::Event::Resized:
			setScale();
			break;
		}
	}

	void setScale() {
		sf::Vector2u winSize = m_window.getSize();

		sf::Vector2u size = rankingPanel.getTexture()->getSize();

		double sx = (double)winSize.x * 0.6 / size.x;
		double sy = (double)winSize.y * 0.65 / size.y;

		double scale = std::min(sx, sy);

		rankingPanel.setScale(scale, scale);
		rankingPanel.setPosition(0, winSize.y * 0.125);


		sf::Vector2u bgSize = bgTexture.getSize();
		sx = (double)winSize.x / (double)bgSize.x;
		sy = (double)winSize.y / (double)bgSize.y;
		scale = std::max(sx, sy);

		bg.setOrigin(bgSize.x / 2.0, bgSize.y / 2.0);
		bg.setPosition(winSize.x / 2.0, winSize.y / 2.0);
		bg.setScale(scale, scale);

		sf::Rect<int> BackgroundRect(0, 0, bgSize.x, bgSize.y);
		bg.setTextureRect(BackgroundRect);

		topFilter.setSize(sf::Vector2f(winSize.x, winSize.y * 0.125));

		Line1.setPosition(5, 5);
		Line2.setPosition(5, winSize.y * 0.05);
		Line3.setPosition(5, winSize.y * 0.075);

		Line1.setCharacterSize(winSize.y * 0.5 * 0.08);
		Line2.setCharacterSize(winSize.y * 0.5 * 0.05);
		Line3.setCharacterSize(winSize.y * 0.5 * 0.05);

		rankingTitle.setPosition(sf::Vector2f(winSize.x - (800 * 0.4 * winSize.y / 600), 0));
		size = rankingTitle.getTexture()->getSize();

		sy = (winSize.y / 600 * 0.175) / size.y;

		rankingTitle.setScale(sy, sy);
	}

public:
	Results(sf::RenderWindow& window, UI *from, DB::gameDB DB, std::unordered_map<std::string, std::string> bmData, Scoring::Score rec) :
		UI(window, from), cur(window), gameDB(DB), beatmapData(bmData), record(rec)
	{
		std::string base_dir = "D:/osu!/Songs/";
		base_dir += beatmapData["OsuDir"] + "/";

		bmPlay.load(base_dir + beatmapData["OsuFile"]);

		font.loadFromFile("resource\\Chakra-Petch-master\\fonts\\ChakraPetch-Medium.ttf");

		Line1.setFont(font);
		Line2.setFont(font);
		Line3.setFont(font);

		Line1.setFillColor(sf::Color::White);
		Line2.setFillColor(sf::Color::White);
		Line3.setFillColor(sf::Color::White);

		Line1.setString(bmPlay.Metadata.Artist + " - " + bmPlay.Metadata.Title + " [" + bmPlay.Metadata.Version + "]");
		Line2.setString("Beatmap by " + bmPlay.Metadata.Creator);
		Line3.setString("Played by " + rec.User + " on " + rec.time + " UTC .");

		rankingPanel.setTexture(*Skin::get("ranking-panel"));

		if (!bgTexture.loadFromFile(base_dir + bmPlay.Events.Background))
			bg.setColor(sf::Color(0, 0, 0, 0));
		else
			bg.setColor(sf::Color(255,255,255,128));
		bgTexture.setSmooth(true);
		bg.setTexture(bgTexture);

		topFilter.setFillColor(sf::Color(0, 0, 0, 200));

		ranking.setTexture(*Skin::get("ranking-" + Scoring::ScoreRank::tostring(rec.Rank)));
		rankingTitle.setTexture(*Skin::get("ranking-title"));
		

		setScale();
	}
};