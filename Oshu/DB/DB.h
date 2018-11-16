#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
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
			std::cerr << "Error SQLite3 database: " << rc << " : " << in << " : " << sqlite3_errmsg(db) << std::endl << std::endl;
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
		do {
			rc = sqlite3_step(stmt);
			switch (rc) {
			case SQLITE_DONE:
				break;
			case SQLITE_ROW:
				count++;
				break;
			default:
				sqlite3_finalize(stmt);
				haveErr("update result");
				return 1;
				break;
			}
		} while (rc == SQLITE_ROW);
		

		sqlite3_reset(stmt);
		sqlite3_clear_bindings(stmt);

		return count;
	}

	bool haveTable(std::string table) {
		sqlite3_stmt *checkStmt;
		const char *sqlCheckTable = "SELECT name FROM sqlite_master WHERE type='table' AND name=?;";

		rc = sqlite3_prepare_v2(db, sqlCheckTable, strlen(sqlCheckTable), &checkStmt, nullptr);
		if (haveErr("check table")) return 1;

		sqlite3_bind_text(checkStmt, 1, table.c_str(), table.size(), 0);

		return countRow(checkStmt) >= 1;
	}

public:
	~DB() {
		sqlite3_close(db);
	}

	int open(std::string dbFile = "") {
		if (dbFile == "") dbFile = openFile;

		rc = sqlite3_open(dbFile.c_str(), &db);
		if (haveErr("opening")) return 1;

		isOpen = true;
		openFile = dbFile;
		return 0;
	}

	std::vector<std::unordered_map<std::string, std::string>> *getData(sqlite3_stmt *stmt) {
		std::vector<std::unordered_map<std::string, std::string>> *Data;
		Data = new std::vector<std::unordered_map<std::string, std::string>>;
		while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
			std::unordered_map<std::string, std::string> row;
			int num_cols = sqlite3_column_count(stmt);
			for (int i = 0; i < num_cols; i++) {
				row[sqlite3_column_name(stmt, i)] = (char *)sqlite3_column_text(stmt, i);
			}
			Data->push_back(row);
		}

		sqlite3_reset(stmt);
		sqlite3_clear_bindings(stmt);

		return Data;
	}
};