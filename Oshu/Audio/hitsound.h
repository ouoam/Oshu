#pragma once

#include <string>
#include <fstream>

#include <SFML/Audio.hpp>

#include "../Beatmap/Beatmap.h"
#include "../Utility/file.h"
#include "../Object/Container.h"

namespace Audio {

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

bool loadSound(sf::SoundBuffer *sb, std::string file) {
	sf::FileInputStream stream;
	if (file_exists(file)) {
		if (stream.open(file)) {
			if (stream.getSize() > 64) {
				try {
					sb->loadFromFile(file);
					return true;
				}
				catch (const std::exception& e) {
					std::cout << "Load Sound Error : " << e.what() << std::endl;
				}
			}
		}
	}
	return false;
}

void loadHitSound(Beatmap::Beatmap *bmPlay, std::string base_dir) {
	int iTP = 0;
	for (int i = 0; i < bmPlay->HitObjects.size(); i++) {
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
			std::string end = (index != 0 ? std::to_string(index) : "");
			sf::SoundBuffer *sb = &hitSoundList[sampleset][sound][index];
			if (!loadSound(sb, base_dir + file + end + ".wav"))
				// DISABLE CUSTOM SKIN
				//if (!loadSound(sb, "resource/Modify/" + file + ".wav"))
					if (!loadSound(sb, "resource/default/" + file + ".wav"))
						std::cout << "Can not load any sound : " << file << end << std::endl;
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

void playHitSound(Beatmap::Beatmap *bmPlay, Object::ContainerHitObject *obj) {
	int sampleset = bmPlay->TimingPoints[std::max(bmPlay->iTimingPoints - 1, 0)].SampleSet;
	int sound_id = obj->hitObject->hitSound;
	int index = bmPlay->TimingPoints[std::max(bmPlay->iTimingPoints - 1, 0)].SampleIndex;
	sound_id = (sound_id & 8) ? 3 : ((sound_id & 4) ? 2 : ((sound_id & 2) ? 1 : 0));
	sound[iSound % 10].setBuffer(hitSoundList[sampleset][sound_id][index]);
	sound[iSound % 10].play();
	sound[iSound % 10].setPlayingOffset(sf::seconds(0));
	iSound++;
}

}