#pragma once

#include <filesystem>

#include "DB.h"
#include "../Beatmap/Beatmap.h"

class beatmapDB : public DB {
private:

	bool addBeatmap(std::filesystem::path beatmap) {
		std::string path = beatmap.parent_path().filename().string();
		std::string file = beatmap.filename().string();

		sqlite3_stmt *insertStmt;

		std::string insertSQL = "INSERT INTO songs (Title, TitleUnicode, Artist, ArtistUnicode, Creator, Version, Source, "
			"Tags, BeatmapID, BeatmapSetID, AudioFilename, AudioLeadIn, PreviewTime, HPDrainRate, CircleSize, "
			"OverallDifficulty, ApproachRate, SliderMultiplier, OsuFile, OsuDir, nHitcircles, nSlider, "
			"nSplinners) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

		rc = sqlite3_prepare_v2(db, insertSQL.c_str(), insertSQL.size(), &insertStmt, nullptr);
		if (haveErr("prepair insert")) return 1;

		Beatmap::Beatmap bm(beatmap.string(), false);

		sqlite3_bind_text(insertStmt, 1, bm.Metadata.Title.c_str(), bm.Metadata.Title.size(), 0);
		sqlite3_bind_text(insertStmt, 2, bm.Metadata.TitleUnicode.c_str(), bm.Metadata.TitleUnicode.size(), 0);
		sqlite3_bind_text(insertStmt, 3, bm.Metadata.Artist.c_str(), bm.Metadata.Artist.size(), 0);
		sqlite3_bind_text(insertStmt, 4, bm.Metadata.ArtistUnicode.c_str(), bm.Metadata.ArtistUnicode.size(), 0);
		sqlite3_bind_text(insertStmt, 5, bm.Metadata.Creator.c_str(), bm.Metadata.Creator.size(), 0);
		sqlite3_bind_text(insertStmt, 6, bm.Metadata.Version.c_str(), bm.Metadata.Version.size(), 0);
		sqlite3_bind_text(insertStmt, 7, bm.Metadata.Source.c_str(), bm.Metadata.Source.size(), 0);
		sqlite3_bind_text(insertStmt, 8, bm.Metadata.Tags.c_str(), bm.Metadata.Tags.size(), 0);
		sqlite3_bind_int(insertStmt, 9, bm.Metadata.BeatmapID);
		sqlite3_bind_int(insertStmt, 10, bm.Metadata.BeatmapSetID);

		sqlite3_bind_text(insertStmt, 11, bm.General.AudioFilename.c_str(), bm.General.AudioFilename.size(), 0);
		sqlite3_bind_int(insertStmt, 12, bm.General.AudioLeadIn);
		sqlite3_bind_int(insertStmt, 13, bm.General.PreviewTime);

		sqlite3_bind_double(insertStmt, 14, bm.Difficulty.HPDrainRate);
		sqlite3_bind_double(insertStmt, 15, bm.Difficulty.CircleSize);
		sqlite3_bind_double(insertStmt, 16, bm.Difficulty.OverallDifficulty);
		sqlite3_bind_double(insertStmt, 17, bm.Difficulty.ApproachRate);
		sqlite3_bind_double(insertStmt, 18, bm.Difficulty.SliderMultiplier);

		sqlite3_bind_text(insertStmt, 19, file.c_str(), file.size(), 0);
		sqlite3_bind_text(insertStmt, 20, path.c_str(), path.size(), 0);
		sqlite3_bind_int(insertStmt, 21, bm.nHitcircles);
		sqlite3_bind_int(insertStmt, 22, bm.nSlider);
		sqlite3_bind_int(insertStmt, 23, bm.nSplinners);

		rc = sqlite3_step(insertStmt);
		sqlite3_finalize(insertStmt);

		if (haveErr("prepair insert")) return 1;
		return 0;
	}

public:

	beatmapDB() {
		openFile = "songs.db";
		path = "D:/osu!/Songs/";
	}

	int create() {
		if (!checkOpen()) return 2;

		const char *sqlCreateTable =
			"CREATE TABLE songs ("
			"id	INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
			"Title	TEXT,"
			"TitleUnicode	TEXT,"
			"Artist	TEXT,"
			"ArtistUnicode	TEXT,"
			"Creator	TEXT,"
			"Version	TEXT,"
			"Source	TEXT,"
			"Tags	TEXT,"
			"BeatmapID	NUMERIC,"
			"BeatmapSetID	NUMERIC,"
			"AudioFilename	TEXT,"
			"AudioLeadIn	NUMERIC,"
			"PreviewTime	NUMERIC,"
			"HPDrainRate	REAL,"
			"CircleSize	REAL,"
			"OverallDifficulty	REAL,"
			"ApproachRate	REAL,"
			"SliderMultiplier	REAL,"
			"OsuFile	TEXT,"
			"OsuDir	TEXT,"
			"nHitcircles	NUMERIC,"
			"nSlider	NUMERIC,"
			"nSplinners	NUMERIC"
			");";
		rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);

		if (haveErr("create")) return 1;
		return 0;
	}

	int update() {
		if (!checkOpen()) return 2;

		sqlite3_stmt *findStmt;
		std::string findSQL = "SELECT id FROM songs WHERE OsuFile = ? AND OsuDir = ?;";

		rc = sqlite3_prepare_v2(db, findSQL.c_str(), findSQL.size(), &findStmt, nullptr);

		if (haveErr("update prepare")) return 1;

		namespace fs = std::filesystem;

		for (const auto & beatmapSet : fs::directory_iterator(path)) {
			for (const auto & beatmap : fs::directory_iterator(beatmapSet.path())) {
				if (fs::is_regular_file(beatmap) && beatmap.path().extension() == ".osu") {
					std::string path = beatmapSet.path().filename().string();
					std::string file = beatmap.path().filename().string();

					sqlite3_bind_text(findStmt, 1, file.c_str(), file.size(), 0);
					sqlite3_bind_text(findStmt, 2, path.c_str(), path.size(), 0);

					bool willAdd = true;
					do {
						rc = sqlite3_step(findStmt);
						switch (rc) {
						case SQLITE_DONE:
							break;
						case SQLITE_ROW:
							willAdd = false;
							break;
						default:
							sqlite3_finalize(findStmt);
							break;
						}
						if (haveErr("update result")) return 1;
					} while (rc == SQLITE_ROW);


					if (willAdd) {
						addBeatmap(beatmap.path());
					}

					sqlite3_reset(findStmt);
					sqlite3_clear_bindings(findStmt);
				}
			}
		}
		sqlite3_finalize(findStmt);
	}
};