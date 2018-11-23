#pragma once

#include <string>
#include <fstream>

#include <SFML/Audio.hpp>

#include "../Beatmap/Beatmap.h"
#include "../Utility/file.h"


sf::Sound sound[10];
int iSound = 0;

std::vector <std::vector<std::vector<sf::SoundBuffer>>> hitSoundList(4, std::vector<std::vector<sf::SoundBuffer>>(4, std::vector<sf::SoundBuffer>(101)));

std::string hitNum2String(int sampleset, int sound) {
	std::string out = "";
	switch (sampleset) {
	case 1:
		out += "normal";
		break;
	case 2:
		out += "soft";
		break;
	case 3:
		out += "drum";
		break;
	}
	out += "-hit";
	switch (sound) {
	case 0:
		out += "normal";
		break;
	case 1:
		out += "whistle";
		break;
	case 2:
		out += "finish";
		break;
	case 3:
		out += "clap";
		break;
	}

	return out;
}

void loadHitSound(Beatmap::Beatmap *bmPlay, std::string base_dir) {
	int iTP = 0;
	for (int i = 0; i < bmPlay->HitObjects.size(); i++)
	{
		int time = bmPlay->HitObjects[i].time;
		if (iTP < bmPlay->TimingPoints.size() - 1) {
			if (bmPlay->TimingPoints[iTP].Offset <= time) {
				iTP++;
			}
		}
		int sampleset = bmPlay->TimingPoints[iTP].SampleSet;
		int sound = bmPlay->HitObjects[i].hitSound;
		int index = bmPlay->TimingPoints[iTP].SampleIndex;
		sound = (sound & 8) ? 3 : ((sound & 4) ? 2 : ((sound & 2) ? 1 : 0));
		if (hitSoundList[sampleset][sound][index].getSampleCount() == 0) {
			std::string file = hitNum2String(sampleset, sound);
			std::string end = (index != 0 ? std::to_string(index) : "") + ".wav";
			if (file_exists(base_dir + file + end)) {
				sf::FileInputStream stream;
				if (stream.open(base_dir + file + end))
					if (stream.getSize() > 64) 
					//std::cout << "ddddddddd" << stream.getSize() << std::endl;
						hitSoundList[sampleset][sound][index].loadFromFile(base_dir + file + end);
			}
			else if (file_exists("resource/Modify/" + file + ".wav")) {
				hitSoundList[sampleset][sound][index].loadFromFile("resource/Modify/" + file + ".wav");
			}
			else if (file_exists("resource/default/" + file + ".wav")) {
				hitSoundList[sampleset][sound][index].loadFromFile("resource/default/" + file + ".wav");
			}
			else {
				hitSoundList[sampleset][sound][index].loadFromFile("resource/soft-hitnormal.wav");
			}
		}
	}
}

float mspb = 0;
float oldmspb = 0;


void updateHitsound(Beatmap::Beatmap *bmPlay, int64_t time) {
	// set volume of sound effect
	if (bmPlay->iTimingPoints < bmPlay->TimingPoints.size()) {
		if (bmPlay->TimingPoints[bmPlay->iTimingPoints].Offset <= time) {
			for (int i = 0; i < 10; i++) {
				sound[i].setVolume(bmPlay->TimingPoints[bmPlay->iTimingPoints].Volume);
			}
			if (bmPlay->TimingPoints[bmPlay->iTimingPoints].mspb > 0) {
				mspb = bmPlay->TimingPoints[bmPlay->iTimingPoints].mspb;
				oldmspb = mspb;
			}
			else {
				mspb = oldmspb * -(bmPlay->TimingPoints[bmPlay->iTimingPoints].mspb) / 100;
			}

			bmPlay->iTimingPoints++;
		}
	}
}