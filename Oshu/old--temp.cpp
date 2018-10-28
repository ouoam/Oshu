


		//
		//if (showHitObj.front < bmPlay.HitObjects.size() && nextHit == -1) {
		//	if (bmPlay.HitObjects[showHitObj.front].time <= time) {
		//		if (bmPlay.HitObjects[showHitObj.front].type & 1) { // circle
		//			nextHit = bmPlay.HitObjects[showHitObj.front].time + 70;
		//			showHitObj.front++;
		//		}
		//		else if(bmPlay.HitObjects[showHitObj.front].type & 2) { // slider
		//			int duration = bmPlay.HitObjects[showHitObj.front].sliders.pixelLength / (100.0 * bmPlay.Difficulty.SliderMultiplier) * mspb;
		//			int calcTime = bmPlay.HitObjects[showHitObj.front].time;
		//			int temp = 0;
		//			int i = 0;
		//			while (temp <= (int)time) {
		//				temp = calcTime + 70;
		//				calcTime += duration;
		//				i++;
		//			}
		//			if (i > bmPlay.HitObjects[showHitObj.front].sliders.repeat +1 ) {
		//				showHitObj.front++;
		//			}
		//			else {
		//				nextHit = temp;
		//			}
		//		}
		//		else if (bmPlay.HitObjects[showHitObj.front].type & 8) { // spinner
		//			nextHit = bmPlay.HitObjects[showHitObj.front].endTime + 70;
		//			showHitObj.front++;
		//		}
		//	}
		//}
		//

		//if (time >= nextHit && nextHit != -1)
		//{
		//	/*int sampleset = bmPlay.TimingPoints[bmPlay.iTimingPoints - 1].SampleSet;
		//	int sound_id = bmPlay.HitObjects[showHitObj.front - (showHitObj.front == 0 ? 0 : 1)].hitSound;
		//	int index = bmPlay.TimingPoints[bmPlay.iTimingPoints - 1].SampleIndex;
		//	sound_id = (sound_id & 8) ? 3 : ((sound_id & 4) ? 2 : ((sound_id & 2) ? 1 : 0));
		//	sound[iSound % 10].setBuffer(hitSoundList[sampleset][sound_id][index]);
		//	sound[iSound % 10].play();
		//	sound[iSound % 10].setPlayingOffset(sf::seconds(0));
		//	iSound++;*/
		//	
		//	lastHit = nextHit;
		//	nextHit = -1;
		//	combo++;
		//}




		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
		//{
		//	if (keyPressed == 0)
		//	{
		//		if (showHitObj.front < bmPlay.HitObjects.size()) {
		//			sf::Vector2i position = sf::Mouse::getPosition(window);
		//			int hit = 0;
		//			int isHO = showHitObj.front;
		//			while (hit == 0 && isHO < showHitObj.back) {
		//				int hitTime = bmPlay.HitObjects[isHO].time;
		//				int hitDelay = time - hitTime - 20;
		//				lastDelay = hitDelay;
		//				position -= bmPlay.HitObjects[isHO].position;
		//				position -= sf::Vector2i(80, 80);

		//				int distant = sqrt((position.x * position.x) + (position.y * position.y));
		//				dist = distant;

		//				if (distant <= CircleRadius) {
		//					if (-hitWinWidth.s50 <= hitDelay && hitDelay <= hitWinWidth.s50) {
		//						last = " 50";
		//						if (-hitWinWidth.s100 <= hitDelay && hitDelay <= hitWinWidth.s100) {
		//							last = "100";
		//							if (-hitWinWidth.s300 <= hitDelay && hitDelay <= hitWinWidth.s300) {
		//								last = "300";
		//							}
		//						}
		//						
		//						int sampleset = bmPlay.TimingPoints[bmPlay.iTimingPoints].SampleSet;
		//						int sound_id = bmPlay.HitObjects[isHO].hitSound;
		//						int index = bmPlay.TimingPoints[bmPlay.iTimingPoints].SampleIndex;
		//						sound_id = (sound_id & 8) ? 3 : ((sound_id & 4) ? 2 : ((sound_id & 2) ? 1 : 0));

		//						sound[iSound % 10].setBuffer(hitSoundList[sampleset][sound_id][index]);
		//						sound[iSound % 10].play();
		//						sound[iSound % 10].setPlayingOffset(sf::seconds(0));
		//						iSound++;
		//						

		//						showHitObj.front = isHO + 1;
		//						combo++;

		//						

		//						hit = 1;
		//					}
		//					else {
		//						showHitObj.front++;
		//					}
		//				}

		//				isHO++;
		//			}
		//		}
		//	}
		//	keyPressed = 1;
		//}
		//else
		//	keyPressed = 0;



		////render

		//window.clear(sf::Color::Black);

		

		//

		//// For debug

		//sf::Text text;
		//sf::Font font;
		//font.loadFromFile("arial.ttf");
		//text.setFont(font);
		//text.setString(std::to_string(lastDelay));
		//text.setCharacterSize(16);
		//text.setFillColor(sf::Color::Red);
		//window.draw(text);

		//sf::Text text1;
		//text1.setFont(font);
		//text1.setString(last);
		//text1.setCharacterSize(16);
		//text1.setFillColor(sf::Color::Red);
		//text1.setPosition(0, 20);
		//window.draw(text1);

		//sf::Text text2;
		//text2.setFont(font);
		//text2.setString(std::to_string(combo) + "x");
		//text2.setCharacterSize(16);
		//text2.setFillColor(sf::Color::Red);
		//text2.setPosition(0, 40);
		//window.draw(text2);



		//sf::Text text3;
		//text3.setFont(font);
		//text3.setString("mspb = " + std::to_string(mspb));
		//text3.setCharacterSize(16);
		//text3.setFillColor(sf::Color::Red);
		//text3.setPosition(0, 60);
		//window.draw(text3);

		//sf::Text text4;
		//text4.setFont(font);
		//text4.setString("y = " + std::to_string(localPosition.y));
		//text4.setCharacterSize(16);
		//text4.setFillColor(sf::Color::Red);
		//text4.setPosition(0, 80);
		//window.draw(text4);

		//sf::Text text5;
		//text5.setFont(font);
		//text5.setString("dist = " + std::to_string(dist));
		//text5.setCharacterSize(16);
		//text5.setFillColor(sf::Color::Red);
		//text5.setPosition(0, 100);
		//window.draw(text5);