#include "Beatmap.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "../Utility/string.h"
#include "../Utility/BezierApproximator.h"
#include "../Utility/CircularArc.h"
#include "../Utility/lineThinkness.h"

namespace Beatmap {

	void Beatmap::load(std::string file) {
		std::string line;
		bmFile.open(file);
		if (bmFile.is_open()) {
			while (getline(bmFile, line)) {
				if (line.compare(0, 2, "//") == 0) {
					// This Line Is Comment
					continue;
				}
				if (line[0] == '[' && line[line.size() - 1] == ']') {
					// Open New Sections
					if (line == "[General]") {
						bmProGeneral();
					}
					else if (line == "[Editor]") {
						bmProEditor();
					}
					else if (line == "[Metadata]") {
						bmProMetadata();
					}
					else if (line == "[Difficulty]") {
						bmProDifficulty();
					}
					else if (line == "[Events]") {
						bmProEvents();
					}
					else if (line == "[TimingPoints]") {
						bmProTimingPoints();
					}
					else if (line == "[Colours]") {
						bmProColours();
					}
					else if (line == "[HitObjects]") {
						bmProHitObjects();
					}
				}
			}
			bmFile.close();
		}
	}

	void Beatmap::bmProGeneral() {
		std::string line = "++";
		while (line != "") {
			getline(bmFile, line);
			std::string key;
			std::string value;
			std::istringstream str(line);

			getline(str, key, ':');
			getline(str, value);
			ltrim(value);

			if (key == "AudioFilename") {
				General.AudioFilename = value;
			}
			else if (key == "AudioLeadIn") {
				General.AudioLeadIn = stoi(value);
			}
			else if (key == "PreviewTime") {
				General.PreviewTime = stoi(value);
			}
			else if (key == "Countdown") {
				General.Countdown = (value == "1");
			}
			else if (key == "SampleSet") {
				General.SampleSet = value;
			}
			else if (key == "StackLeniency") {
				General.StackLeniency = stof(value);
			}
			else if (key == "Mode") {
				General.Mode = stoi(value);
			}
			else if (key == "LetterboxInBreaks") {
				General.LetterboxInBreaks = (value == "1");
			}
			else if (key == "WidescreenStoryboard") {
				General.WidescreenStoryboard = (value == "1");
			}
			else if (key == "StoryFireInFront") {
				General.StoryFireInFront = (value == "1");
			}
			else if (key == "SpecialStyle") {
				General.SpecialStyle = (value == "1");
			}
			else if (key == "EpilepsyWarning") {
				General.EpilepsyWarning = (value == "1");
			}
			else if (key == "UseSkinSprites") {
				General.UseSkinSprites = (value == "1");
			}
		}
	}

	void Beatmap::bmProEditor() {
		std::string line = "++";
		while (line != "") {
			getline(bmFile, line);
			std::string key;
			std::string value;
			std::istringstream str(line);

			getline(str, key, ':');
			getline(str, value);
			ltrim(value);

			if (key == "Bookmarks") {
				/* vector<int> Bookmarks; */
			}
			else if (key == "DistanceSpacing") {
				Editor.DistanceSpacing = stof(value);
			}
			else if (key == "BeatDivisor") {
				Editor.BeatDivisor = stoi(value);
			}
			else if (key == "GridSize") {
				Editor.GridSize = stoi(value);
			}
			else if (key == "TimelineZoom") {
				Editor.TimelineZoom = stoi(value);
			}
		}
	}

	void Beatmap::bmProMetadata() {
		std::string line = "++";
		while (line != "") {
			getline(bmFile, line);
			std::string key;
			std::string value;
			std::istringstream str(line);

			getline(str, key, ':');
			getline(str, value);

			if (key == "Title") {
				Metadata.Title = value;
			}
			else if (key == "TitleUnicode") {
				Metadata.TitleUnicode = value;
			}
			else if (key == "Artist") {
				Metadata.Artist = value;
			}
			else if (key == "ArtistUnicode") {
				Metadata.ArtistUnicode = value;
			}
			else if (key == "Creator") {
				Metadata.Creator = value;
			}
			else if (key == "Version") {
				Metadata.Version = value;
			}
			else if (key == "Source") {
				Metadata.Source = value;
			}
			else if (key == "Tags") {
				Metadata.Tags = value;
			}
			else if (key == "BeatmapID") {
				Metadata.BeatmapID = stoi(value);
			}
			else if (key == "BeatmapSetID") {
				Metadata.BeatmapSetID = stoi(value);
			}
		}
	}

	void Beatmap::bmProDifficulty() {
		std::string line = "++";
		while (line != "") {
			getline(bmFile, line);
			std::string key;
			std::string value;
			std::istringstream str(line);

			getline(str, key, ':');
			getline(str, value);

			if (key == "HPDrainRate") {
				Difficulty.HPDrainRate = stof(value);
			}
			else if (key == "CircleSize") {
				Difficulty.CircleSize = stof(value);
			}
			else if (key == "OverallDifficulty") {
				Difficulty.OverallDifficulty = stof(value);
			}
			else if (key == "ApproachRate") {
				Difficulty.ApproachRate = stof(value);
			}
			else if (key == "SliderMultiplier") {
				Difficulty.SliderMultiplier = stof(value);
			}
			else if (key == "SliderTickRate") {
				Difficulty.SliderTickRate = stof(value);
			}

			Difficulty.CircleRadius = 32 * (1 - 0.7 * (Difficulty.CircleSize - 5) / 5);
		}
	}

	void Beatmap::bmProEvents() {
		std::string line = "++";
		while (line != "") {
			getline(bmFile, line);
			std::string key;
			std::string value;
			std::istringstream str(line);

			getline(str, key, ':');
			getline(str, value);

			/* !!!! Wait !!!! */
		}
	}

	void Beatmap::bmProTimingPoints() {
		std::string line = "++";
		getline(bmFile, line);
		while (line != "") {
			std::string value;
			std::istringstream str(line);

			bmTimingPoints bmtp;

			getline(str, value, ',');
			bmtp.Offset = stoi(value);
			getline(str, value, ',');
			bmtp.mspb = stod(value);
			getline(str, value, ',');
			bmtp.Meter = stoi(value);
			getline(str, value, ',');
			bmtp.SampleSet = stoi(value);
			getline(str, value, ',');
			bmtp.SampleIndex = stoi(value);
			getline(str, value, ',');
			bmtp.Volume = stoi(value);
			getline(str, value, ',');
			bmtp.Inherited = (value == "1");
			getline(str, value, ',');
			bmtp.KiaiMode = (value == "1");

			TimingPoints.push_back(bmtp);
			getline(bmFile, line);
		}
	}

	void Beatmap::bmProColours() {
		std::string line = "++";
		while (line != "") {
			getline(bmFile, line);
			std::string key;
			std::string value;
			std::istringstream str(line);

			getline(str, key, ':');
			getline(str, value);
			rtrim(key);
			ltrim(value);

			/* !!!! Wait !!!! */
		}
	}

	void Beatmap::bmProHitObjects() {
		std::string line = "++";
		getline(bmFile, line);
		while (line != "") {
			std::string value;
			std::istringstream str(line);

			bmHitObjects bmho;

			getline(str, value, ',');
			bmho.position.x = stoi(value);
			getline(str, value, ',');
			bmho.position.y = stoi(value);
			getline(str, value, ',');
			bmho.time = stoi(value);
			getline(str, value, ',');
			bmho.type = stoi(value);
			getline(str, value, ',');
			bmho.hitSound = stoi(value);

			if (bmho.type & 2) { // slider
				sf::VertexArray curvePoints;

				curvePoints.append(sf::Vertex((sf::Vector2f)bmho.position));
				getline(str, value, ',');

				std::string sub_1V = "++";
				std::istringstream sub_1S(value);

				getline(sub_1S, sub_1V, '|');
				bmho.sliders.sliderType = sub_1V;
				while (getline(sub_1S, sub_1V, '|')) {
					sf::Vector2i temp;
					std::string sub_2V;
					std::istringstream sub_2S(sub_1V);

					getline(sub_2S, sub_2V, ':');
					temp.x = stoi(sub_2V);
					getline(sub_2S, sub_2V, ':');
					temp.y = stoi(sub_2V);
					curvePoints.append(sf::Vertex((sf::Vector2f)temp));
				}
				bmho.sliders.endPoint = curvePoints[curvePoints.getVertexCount() - 1].position;

				if (bmho.sliders.sliderType == "L") {
					sf::VertexArray line(sf::LineStrip, 2);
					line[0].position = sf::Vector2f(curvePoints[0].position);
					line[1].position = sf::Vector2f(curvePoints[1].position);
					bmho.sliders.curvePoints = line;
				}
				else if (bmho.sliders.sliderType == "P")
				{
					bmho.sliders.curvePoints = CircularArcApproximator(curvePoints).CreateArc();
				}
				else if (bmho.sliders.sliderType == "B")
				{
					bmho.sliders.curvePoints = BezierApproximator(curvePoints).CreateBezier();
				}

				bmho.sliders.lineThinkness = GenerateTrianglesStrip(bmho.sliders.curvePoints, Difficulty.CircleRadius * 2);

				getline(str, value, ',');
				bmho.sliders.repeat = stoi(value);
				getline(str, value, ',');
				bmho.sliders.pixelLength = stof(value);
				getline(str, value, ',');
				bmho.sliders.edgeHitsounds = value;
				getline(str, value, ',');
				bmho.sliders.edgeAddition = value;
			}
			else if (bmho.type & 8) { // spinner
				getline(str, value, ',');
				bmho.endTime = stoi(value);
			}

			// extras
			getline(str, value, ':');
			bmho.extras.sampleSet = stoi(value);
			getline(str, value, ':');
			bmho.extras.additionSet = stoi(value);
			getline(str, value, ':');
			bmho.extras.customIndex = stoi(value);
			getline(str, value, ':');
			bmho.extras.sampleVolume = stoi(value);
			getline(str, value, ':');
			bmho.extras.filename = value;

			HitObjects.push_back(bmho);
			getline(bmFile, line);
		}
	}

}