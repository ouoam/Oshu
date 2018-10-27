//#pragma once
//
//// Use For Hit Windows
//struct hitWinStruct
//{
//	int time;
//	int delay;
//	hitWinStruct(int a, int b) {
//		time = a;
//		delay = b;
//	}
//};
//
//
//std::deque<hitWinStruct> hitWinList;
//
//hitWinList.push_back(hitWinStruct(time - 70, time - nextHit));
//hitWinList.push_back(hitWinStruct(time, hitDelay));
//
//// clear old hit window
//while (!hitWinList.empty() && hitWinList.front().time + 8 * 1000 < time)
//{
//	hitWinList.pop_front();
//}
//
//// hit windows
//int x = 400;
//int y = 620;
//sf::RectangleShape score50(sf::Vector2f(hitWinWidth.s50 * 2, 6));
//score50.setOrigin(hitWinWidth.s50, 3);
//score50.setPosition(x, y);
//score50.setFillColor(sf::Color(218, 174, 70));
//window.draw(score50);
//
//sf::RectangleShape score100(sf::Vector2f(hitWinWidth.s100 * 2, 6));
//score100.setOrigin(hitWinWidth.s100, 3);
//score100.setPosition(x, y);
//score100.setFillColor(sf::Color(87, 227, 19));
//window.draw(score100);
//
//sf::RectangleShape score300(sf::Vector2f(hitWinWidth.s300 * 2, 6));
//score300.setOrigin(hitWinWidth.s300, 3);
//score300.setPosition(x, y);
//score300.setFillColor(sf::Color(50, 180, 231));
//window.draw(score300);
//
//std::deque<int>::size_type sz = hitWinList.size();
//for (unsigned i = 0; i < sz; i++)
//{
//	int delay = hitWinList[i].delay;
//	sf::RectangleShape before(sf::Vector2f(4, 18));
//	before.setOrigin(2, 9);
//	before.setPosition(x + delay, y);
//	before.setFillColor(sf::Color(255, 255, 255, ((hitWinList[i].time + 8 * 1000 - time) / 8.0 / 1000.0) * 127));
//	window.draw(before);
//}
//
//sf::RectangleShape scoreCenter(sf::Vector2f(2, 18));
//scoreCenter.setOrigin(1, 9);
//scoreCenter.setPosition(x, y);
//window.draw(scoreCenter);
//
//// End Hit Windows
//
