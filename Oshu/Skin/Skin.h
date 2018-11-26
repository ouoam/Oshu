#pragma once

#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "../Utility/file.h"

namespace Skin {
	std::string base_dir = "resource/default/";
	std::string now_dir = "";

	class Texture : public sf::Texture {
	private:
		bool loadFile(std::string file) {
			std::string loadFile = "";

			if (file_exists(file + "@2x.png"))
				loadFile = file + "@2x.png";
			else if (file_exists(file + ".png"))
				loadFile = file + ".png";

			if (loadFile != "") {
				try {
					return loadFromFile(loadFile);
				}
				catch (const std::exception& e) {
					std::cout << "Load Texture Error : " << e.what() << std::endl;
				}
			}
			return false;
		}
	public:
		Texture() : sf::Texture::Texture() {}
		Texture(std::string file) : sf::Texture::Texture() {
			loadTexture(file, now_dir);
		}

		void loadTexture(std::string file, std::string dir = "") {
			bool textureLoad = false;
			if (dir != "")
				textureLoad = loadFile(dir + file);
			// DISABLE CUSTOM SKIN
			//if (!textureLoad)
			//	textureLoad = loadFile("resource/Modify/" + file);
			if (!textureLoad)
				textureLoad = loadFile(base_dir + file);
			if (!textureLoad)
				std::cout << "can not load" << std::endl;
			setSmooth(true);
		}
	};

	std::unordered_map<std::string, Texture *> skin;

	Texture *get(std::string file) {
		std::unordered_map<std::string, Texture *>::const_iterator it = skin.find(file);
		if (it == skin.end()) {
			skin[file] = new Texture(file);
		}
		return skin[file];
	}

	void load(std::string path = "") {
		now_dir = path;
		for (auto &val : skin) {
			delete val.second;
			val.second = new Texture(val.first);
		}
	}

	//WHY SQLite will be error
	sf::Texture dbWillErrorIfDontHaveThis;
}