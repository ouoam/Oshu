#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "DB.h"
#include "../Scoring/Score.h"
#include "../Utility/stl.h"

namespace DB {

class ScoringDB : public virtual DB {
private:
	int createSongDB() {
		if (!checkOpen()) return 2;

		const char *sqlCreateTable =
			"CREATE TABLE score ("
			"id			INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,"
			"User		TEXT,"
			"BeatmapID	INTEGER,"
			"Rank		INTEGER,"
			"TotalScore	REAL,"
			"Accuracy	REAL,"
			"MaxCombo	INTEGER,"
			"StatMiss	INTEGER,"
			"StatMeh	INTEGER,"
			"StatGood	INTEGER,"
			"StatGreat	INTEGER,"
			"Time		TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
			");";
		rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);

		if (haveErr("create", false)) return 1;
		return 0;
	}

	std::vector<Scoring::Score*> *BeatmapScoreData = nullptr;
	std::string *User = new std::string;

public:
	ScoringDB(std::string dbFile) : DB(dbFile) {
		checkHaveTable();
	}

	ScoringDB() : ScoringDB("score.db") {}

	void setUser(std::string usertochange) {
		delete User;
		User = new std::string;
		(*User) = usertochange;
	}

	void checkHaveTable() {
		if (!haveTable("score")) createSongDB();
	}

	std::vector<Scoring::Score*> *getBeatmapScore(int id) {
		// Memory leak
		if (BeatmapScoreData != nullptr) {
			for (Scoring::Score* row : *BeatmapScoreData) {
				delete row;
			}
			delete BeatmapScoreData;
		}

		sqlite3_stmt *getBeatmapScoreStmt;
		std::string getSQL = "SELECT * "
			"FROM (SELECT * FROM score WHERE BeatmapID = ? ORDER BY TotalScore ASC, MaxCombo ASC, Time DESC) "
			"GROUP BY User ORDER BY TotalScore DESC, MaxCombo DESC, Time ASC;";

		rc = sqlite3_prepare_v2(db, getSQL.c_str(), getSQL.size(), &getBeatmapScoreStmt, nullptr);

		if (haveErr("getBeatmapScore prepare")) return BeatmapScoreData;

		sqlite3_bind(getBeatmapScoreStmt, 1, id);
		std::vector<std::unordered_map<std::string, std::string>*> *tempData = getData(getBeatmapScoreStmt);
		BeatmapScoreData = new std::vector<Scoring::Score*>;

		for (std::unordered_map<std::string, std::string>* ranking : *tempData) {
			Scoring::Score *score = new Scoring::Score;

			score->User = GetWithDef(*ranking, "User", "--Oshu User--");
			score->BeatmapID = std::stoi(GetWithDef(*ranking, "BeatmapID", "-1"));

			score->Rank = (Scoring::ScoreRank::Enum)std::stoi(GetWithDef(*ranking, "Rank", "0"));
			score->TotalScore = std::stod(GetWithDef(*ranking, "TotalScore", "0"));
			score->Accuracy = std::stod(GetWithDef(*ranking, "TotalScore", "0"));
			score->MaxCombo = std::stoi(GetWithDef(*ranking, "MaxCombo", "0"));

			score->Statistics[Scoring::HitResult::Miss] = std::stoi(GetWithDef(*ranking, "StatMiss", "0"));
			score->Statistics[Scoring::HitResult::Meh] = std::stoi(GetWithDef(*ranking, "StatMeh", "0"));
			score->Statistics[Scoring::HitResult::Good] = std::stoi(GetWithDef(*ranking, "StatGood", "0"));
			score->Statistics[Scoring::HitResult::Great] = std::stoi(GetWithDef(*ranking, "StatGreat", "0"));

			BeatmapScoreData->push_back(score);
		}

		for (std::unordered_map<std::string, std::string>* row : *tempData) {
			delete row;
		}
		delete tempData;

		return BeatmapScoreData;
	}

	bool addScoreToDB(Scoring::Score score) {
		sqlite3_stmt *insertStmt;

		const char insertSQL[] = "INSERT INTO score (User, BeatmapID, Rank, TotalScore, Accuracy,"
			"MaxCombo, StatMiss, StatMeh, StatGood, StatGreat) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

		rc = sqlite3_prepare_v2(db, insertSQL, strlen(insertSQL), &insertStmt, nullptr);
		if (haveErr("prepair insert")) return 1;

		sqlite3_bind(insertStmt, 1, User);
		sqlite3_bind(insertStmt, 2, score.BeatmapID);
		sqlite3_bind(insertStmt, 3, score.Rank);
		sqlite3_bind(insertStmt, 4, score.TotalScore);
		sqlite3_bind(insertStmt, 5, score.Accuracy);
		sqlite3_bind(insertStmt, 6, score.MaxCombo);
		sqlite3_bind(insertStmt, 7, score.Statistics[Scoring::HitResult::Miss]);
		sqlite3_bind(insertStmt, 8, score.Statistics[Scoring::HitResult::Meh]);
		sqlite3_bind(insertStmt, 9, score.Statistics[Scoring::HitResult::Good]);
		sqlite3_bind(insertStmt, 10, score.Statistics[Scoring::HitResult::Great]);

		if ((rc = sqlite3_step(insertStmt)) != SQLITE_DONE) {
			haveErr("inserted", false);
		}
		else {
			std::cout << "Add Score to DB : " << score.BeatmapID << std::endl;
		}
		sqlite3_finalize(insertStmt);

		return 0;
	}
};

}