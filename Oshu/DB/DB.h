#pragma once

#include <string>
#include <iostream>
#include "sqlite3.h"

class DB {
protected:
	sqlite3 *db;

	int rc;
	char *error;
	bool isOpen = false;

	std::string openFile = "";

	bool haveErr(std::string in = "", bool closeIfErr = true) {
		if (rc != SQLITE_OK) {
			std::cerr << "Error SQLite3 database: " << in << " : " << sqlite3_errmsg(db) << std::endl << std::endl;
			if (closeIfErr) {
				sqlite3_close(db);
				isOpen = false;
			}
			return true;
		}
		return false;
	}

	bool checkOpen() {
		if (!isOpen) {
			open();
		}
		return isOpen;
	}

	int countRow(sqlite3_stmt *stmt) {
		int count = 0;
		while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
			count++;
			if (haveErr("update result")) return 1;
		}

		sqlite3_reset(stmt);
		sqlite3_clear_bindings(stmt);

		return count;
	}

public:
	int open(std::string dbFile = "") {
		if (dbFile == "") dbFile = openFile;

		rc = sqlite3_open(dbFile.c_str(), &db);
		if (haveErr("opening")) return 1;

		isOpen = true;
		openFile = dbFile;
		return 0;
	}
};