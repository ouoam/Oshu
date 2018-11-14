#pragma once

#include <string>
#include <vector>
#include <fstream>


#include <SFML/Graphics.hpp>

namespace Beatmap {

	struct bmGeneral {
		std::string AudioFilename;
		int AudioLeadIn;
		int PreviewTime;
		bool Countdown;
		std::string SampleSet;
		float StackLeniency;
		int Mode;
		bool LetterboxInBreaks;
		bool WidescreenStoryboard;
		bool StoryFireInFront;
		bool SpecialStyle;
		bool EpilepsyWarning;
		bool UseSkinSprites;
	};

	struct bmEditor {
		std::vector<int> Bookmarks;
		float DistanceSpacing;
		int BeatDivisor;
		int GridSize;
		int TimelineZoom;
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
		int BeatmapID;
		int BeatmapSetID;
	};

	struct bmDifficulty {
		double HPDrainRate;
		double CircleSize;
		double OverallDifficulty;
		double ApproachRate;
		double SliderMultiplier;
		double SliderTickRate;

		double CircleRadius;
	};

	struct nmEvents {
		// Wait !!!!!
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

		static int TimePreempt;
		static int TimeFadeIn;
		static int CR;
	};

	class Beatmap {
		FILE * bmFile;
		void bmProGeneral();
		void bmProEditor();
		void bmProMetadata();
		void bmProDifficulty();
		void bmProEvents();
		void bmProTimingPoints();
		void bmProColours();
		void bmProHitObjects(bool calcCurve);

		char * buffer = nullptr;

	public:
		struct bmGeneral General;
		struct bmEditor Editor;
		struct bmMetadata Metadata;
		struct bmDifficulty Difficulty;
		struct nmEvents Events;
		std::vector<struct bmTimingPoints> TimingPoints;
		struct bmColours Colours;
		std::vector<struct bmHitObjects> HitObjects;

		int iTimingPoints = 0;
		int iHitObjects = 0;

		int nHitcircles = 0;
		int nSlider = 0;
		int nSplinners = 0;

		void load(std::string, bool calcCurve = true);
		Beatmap(std::string file, bool calcCurve = true) {
			load(file, calcCurve);
		}
		Beatmap() {}
	};
}

