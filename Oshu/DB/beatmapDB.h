#pragma once

#include <filesystem>

#include "../Utility/string.h"

#include "DB.h"
#include "../Beatmap/Beatmap.h"

namespace DB {

class beatmapDB : public virtual DB {
private:

	bool addBeatmapToDB(std::filesystem::path beatmap) {
		std::string path = beatmap.parent_path().filename().string();
		std::string file = beatmap.filename().string();

		sqlite3_stmt *insertStmt;

		const char insertSQL[] = "INSERT INTO songs (Title, TitleUnicode, Artist, ArtistUnicode, Creator, Version, Source, "
			"Tags, BeatmapID, BeatmapSetID, AudioFilename, AudioLeadIn, PreviewTime, Mode, HPDrainRate, CircleSize, "
			"OverallDifficulty, ApproachRate, SliderMultiplier, OsuFile, OsuDir, nHitcircles, nSlider, "
			"nSplinners) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

		rc = sqlite3_prepare_v2(db, insertSQL, strlen(insertSQL), &insertStmt, nullptr);
		if (haveErr("prepair insert")) return 1;

		Beatmap::Beatmap bm(beatmap.string(), true, false);

		sqlite3_bind(insertStmt, 1, &bm.Metadata.Title);
		sqlite3_bind(insertStmt, 2, &bm.Metadata.TitleUnicode);
		sqlite3_bind(insertStmt, 3, &bm.Metadata.Artist);
		sqlite3_bind(insertStmt, 4, &bm.Metadata.ArtistUnicode);
		sqlite3_bind(insertStmt, 5, &bm.Metadata.Creator);
		sqlite3_bind(insertStmt, 6, &bm.Metadata.Version);
		sqlite3_bind(insertStmt, 7, &bm.Metadata.Source);
		sqlite3_bind(insertStmt, 8, &bm.Metadata.Tags);
		sqlite3_bind(insertStmt, 9, bm.Metadata.BeatmapID);
		sqlite3_bind(insertStmt, 10, bm.Metadata.BeatmapSetID);

		sqlite3_bind(insertStmt, 11, &bm.General.AudioFilename);
		sqlite3_bind(insertStmt, 12, bm.General.AudioLeadIn);
		sqlite3_bind(insertStmt, 13, bm.General.PreviewTime);
		sqlite3_bind(insertStmt, 14, bm.General.Mode);

		sqlite3_bind(insertStmt, 15, bm.Difficulty.HPDrainRate);
		sqlite3_bind(insertStmt, 16, bm.Difficulty.CircleSize);
		sqlite3_bind(insertStmt, 17, bm.Difficulty.OverallDifficulty);
		sqlite3_bind(insertStmt, 18, bm.Difficulty.ApproachRate);
		sqlite3_bind(insertStmt, 19, bm.Difficulty.SliderMultiplier);

		sqlite3_bind(insertStmt, 20, &file);
		sqlite3_bind(insertStmt, 21, &path);
		sqlite3_bind(insertStmt, 22, bm.nHitcircles);
		sqlite3_bind(insertStmt, 23, bm.nSlider);
		sqlite3_bind(insertStmt, 24, bm.nSplinners);

		if ((rc = sqlite3_step(insertStmt)) != SQLITE_DONE) {
			haveErr("inserted", false);
		}
		else {
			std::cout << "Add to DB : " << file << std::endl;
		}
		sqlite3_finalize(insertStmt);

		return 0;
	}

	int createSongDB() {
		if (!checkOpen()) return 2;

		const char *sqlCreateTable =
			"CREATE TABLE songs ("
			"id					INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
			"Title				TEXT,"
			"TitleUnicode		TEXT,"
			"Artist				TEXT,"
			"ArtistUnicode		TEXT,"
			"Creator			TEXT,"
			"Version			TEXT,"
			"Source				TEXT,"
			"Tags				TEXT,"
			"BeatmapID			INTEGER,"
			"BeatmapSetID		INTEGER,"
			"AudioFilename		TEXT,"
			"AudioLeadIn		INTEGER,"
			"PreviewTime		INTEGER,"
			"Mode				INTEGER,"
			"HPDrainRate		REAL,"
			"CircleSize			REAL,"
			"OverallDifficulty	REAL,"
			"ApproachRate		REAL,"
			"SliderMultiplier	REAL,"
			"OsuFile			TEXT,"
			"OsuDir				TEXT,"
			"nHitcircles		NUMERIC,"
			"nSlider			NUMERIC,"
			"nSplinners			NUMERIC"
			");";
		rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);

		if (haveErr("create", false)) return 1;
		return 0;
	}

	std::vector<std::unordered_map<std::string, std::string>*> *searchSongData = nullptr;
	std::vector<std::unordered_map<std::string, std::string>*> *beatmapSetData = nullptr;

public:
	std::string songsPath = "D:/osu!/Songs/";

	beatmapDB(std::string dbFile) : DB(dbFile) {
		checkHaveTable();
	}

	beatmapDB() : beatmapDB("songs.db") {}

	void checkHaveTable() {
		if (!haveTable("songs")) createSongDB();
	}

	int updateSong() {
		if (!checkOpen()) return 2;

		sqlite3_stmt *findStmt;
		const char findSQL[] = "SELECT id FROM songs WHERE OsuFile = ? AND OsuDir = ?;";

		rc = sqlite3_prepare_v2(db, findSQL, strlen(findSQL), &findStmt, nullptr);

		if (haveErr("update prepare")) return 1;

		namespace fs = std::filesystem;

		for (const auto & beatmapSet : fs::directory_iterator(songsPath)) {
			for (const auto & beatmap : fs::directory_iterator(beatmapSet.path())) {
				if (fs::is_regular_file(beatmap) && beatmap.path().extension() == ".osu") {
					std::string path = beatmapSet.path().filename().string();
					std::string file = beatmap.path().filename().string();

					sqlite3_bind(findStmt, 1, &file);
					sqlite3_bind(findStmt, 2, &path);

					if (countRow(findStmt) == 0) {
						addBeatmapToDB(beatmap.path());
					}
				}
			}
		}
		sqlite3_finalize(findStmt);
	}

	std::vector<std::unordered_map<std::string, std::string>*> *searchSong(std::string keyword = "") {
		// Memory leak
		if (searchSongData != nullptr) {
			for (std::unordered_map<std::string, std::string>* row : *searchSongData) {
				delete row;
			}
			delete searchSongData;
		}

		std::vector<std::string> split = explode(keyword, ' ');

		sqlite3_stmt *searchStmt;
		std::string searchSQL = "SELECT id, Title,Artist, Creator FROM ( ";
		searchSQL += "SELECT min(id) as id, Title, Artist, Creator, min(Mode) as Mode FROM songs ";
		if (keyword != "") {
			searchSQL += "WHERE ";
			for (int i = 0; i < split.size(); i++) {
				searchSQL += "(Title like '%' || ? || '%' OR TitleUnicode like '%' || ? || '%' "
					"OR Artist like '%' || ? || '%' OR ArtistUnicode like '%' || ? || '%' "
					"OR Creator like '%' || ? || '%' OR Version like '%' || ? || '%' "
					"OR Source like '%' || ? || '%' OR Tags like '%' || ? || '%') AND ";
			}
			searchSQL.pop_back(); //
			searchSQL.pop_back(); // D
			searchSQL.pop_back(); // N
			searchSQL.pop_back(); // A
		}
		searchSQL += "GROUP BY OsuDir) WHERE Mode = 0 ORDER BY lower(Title);";

		rc = sqlite3_prepare_v2(db, searchSQL.c_str(), searchSQL.size(), &searchStmt, nullptr);

		if (haveErr("search prepare")) {
			searchSongData = new std::vector<std::unordered_map<std::string, std::string>*>;
			return searchSongData;
		}

		if (keyword != "") {
			for (int i = 0; i < split.size(); i++) {
				for (int j = 0; j < 8; j++) {
					sqlite3_bind(searchStmt, i * 8 + j + 1, &split[i]);
				}
			}
		}

		searchSongData = getData(searchStmt);

		return searchSongData;
	}

	std::vector<std::unordered_map<std::string, std::string>*> *getBeatmapSet(int id) {
		// Memory leak
		if (beatmapSetData != nullptr) {
			for (std::unordered_map<std::string, std::string>* row : *beatmapSetData) {
				delete row;
			}
			delete beatmapSetData;
		}

		sqlite3_stmt *getBeatmapSetStmt;
		std::string getSQL = "SELECT * FROM songs WHERE OsuDir = (SELECT OsuDir FROM songs WHERE id = ?) AND Mode = 0 ;";

		rc = sqlite3_prepare_v2(db, getSQL.c_str(), getSQL.size(), &getBeatmapSetStmt, nullptr);

		if (haveErr("getBeatmapSet prepare")) return beatmapSetData;

		sqlite3_bind(getBeatmapSetStmt, 1, id);

		beatmapSetData = getData(getBeatmapSetStmt);

		return beatmapSetData;
	}
};

}