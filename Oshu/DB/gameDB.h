#pragma once

#include "DB.h"
#include "beatmapDB.h"
#include "ScoringDB.h"

namespace DB {

class gameDB : public ScoringDB, public beatmapDB {
public:
	gameDB(std::string fileDB) : DB(fileDB) {
		ScoringDB::checkHaveTable();
		beatmapDB::checkHaveTable();
	}

	gameDB() : gameDB("game.db") { }
};

}