#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstdio>

#include <SFML/Graphics.hpp>

namespace Beatmap {

	struct bmGeneral {
		std::string AudioFilename;
		int AudioLeadIn = 0;
		int PreviewTime = 0;
		bool Countdown = false;
		std::string SampleSet;
		float StackLeniency = 0;
		int Mode = 0;
		bool LetterboxInBreaks = false;
		bool WidescreenStoryboard = false;
		bool StoryFireInFront = false;
		bool SpecialStyle = false;
		bool EpilepsyWarning = false;
		bool UseSkinSprites = false;
	};

	struct bmEditor {
		std::vector<int> Bookmarks;
		float DistanceSpacing = 1;
		int BeatDivisor = 4;
		int GridSize = 4;
		int TimelineZoom = 1;
	};

	struct bmMetadata {
		std::string Title;
		std::string TitleUnicode;
		std::string Artist;
		std::string ArtistUnicode;
		std::string Creator;
		std::string Version;
		std::string Source;
		std::string Tags;
		int BeatmapID = -1;
		int BeatmapSetID = -1;
	};

	struct bmDifficulty {
		double HPDrainRate = 5;
		double CircleSize = 5;
		double OverallDifficulty = 5;
		double ApproachRate = -1;
		double SliderMultiplier = 1;
		double SliderTickRate = 1;

		double CircleRadius = 0;
	};

	struct bmEvents {
		// Wait !!!!!
		std::string Background = "";
		std::string Video = "";
		int VideoOffset = 0;
	};

	struct bmTimingPoints {
		int Offset;
		double mspb;
		int Meter;
		int SampleSet;
		int SampleIndex;
		int Volume;
		bool Inherited;
		bool KiaiMode;
	};

	struct bmColours {
		// Wait !!!!!
	};

	struct bmHitObjects {
		sf::Vector2i position;
		int time;
		int type;
		int hitSound;
		struct {
			int sampleSet;
			int additionSet;
			int customIndex;
			int sampleVolume;
			std::string filename;
		} extras;

		struct {
			std::string sliderType;
			sf::VertexArray curvePoints;
			int repeat;
			float pixelLength;
			std::string edgeHitsounds;
			std::string edgeAddition;
			sf::Vector2f endPoint;
		} sliders;

		int endTime;

		static double TimePreempt;
		static double TimeFadeIn;
		static int CR;
	};

	class Beatmap {
		std::ifstream bmFile;
		void bmProGeneral();
		void bmProEditor();
		void bmProMetadata();
		void bmProDifficulty();
		void bmProEvents();
		void bmProTimingPoints();
		void bmProColours();
		void bmProHitObjects(bool calcCurve);

	public:
		struct bmGeneral General;
		struct bmEditor Editor;
		struct bmMetadata Metadata;
		struct bmDifficulty Difficulty;
		struct bmEvents Events;
		std::vector<struct bmTimingPoints> TimingPoints;
		struct bmColours Colours;
		std::vector<struct bmHitObjects> HitObjects;

		int iTimingPoints = 0;
		int iHitObjects = 0;

		int nHitcircles = 0;
		int nSlider = 0;
		int nSplinners = 0;

		void load(std::string, bool readHO = true, bool calcCurve = true);
		Beatmap(std::string file, bool readHO = true, bool calcCurve = true) {
			load(file, readHO, calcCurve);
		}
		Beatmap() {}
	};
}

