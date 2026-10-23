#include "../headers/database.hpp"
#include "../headers/solve.hpp"
#include "../headers/session.hpp"

#include <sqlite3.h>

#include <iostream>
#include <string>
#include <vector>

int Database::getCurrentSessionId() const{ return current_session_id; }

int Database::getCurrentSolvesAmount() const{ return current_solve_amount; }

int Database::getMaxId() const{ return max_id; }

sqlite3 *Database::openDatabase(){
    sqlite3 *db = nullptr;
    if(sqlite3_open("cube_timer.db", &db) != SQLITE_OK){
        std::cerr << "Error: " << sqlite3_errmsg(db) << std::endl;

        if(db != nullptr){ sqlite3_close(db); }
        return nullptr;
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON", nullptr, nullptr, nullptr);
    return db;
}

void Database::closeDatabase(sqlite3 *db){ if(db != nullptr){ sqlite3_close(db); } }

void Database::createTables(sqlite3 *db){
    const char *sql = R"(
        CREATE TABLE IF NOT EXISTS sessions (
            id INTEGER PRIMARY KEY,
            event INTEGER NOT NULL,
            date TEXT
        );

        CREATE TABLE IF NOT EXISTS solves (
            id INTEGER PRIMARY KEY,
            session_id INTEGER NOT NULL, 
            scramble TEXT, 
            penalty INTEGER NOT NULL DEFAULT 0, 
            raw_time REAL NOT NULL, 
            final_time REAL NOT NULL,
            date TEXT,
            FOREIGN KEY(session_id) REFERENCES sessions(id)
        );
    )";

    char *error = nullptr;
    if(sqlite3_exec(db, sql, nullptr, nullptr, &error) != SQLITE_OK){
        std::cerr << "Error SQL(in createTables): " << error << std::endl;
        sqlite3_free(error);
    }
}

void Database::saveSolve(sqlite3 *db, Solve &solve){
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO solves (session_id, scramble, penalty, raw_time, final_time, date) VALUES (?, ?, ?, ?, ?, ?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if(rc != SQLITE_OK){
        std::cerr << "Error while preparing the consult(in saveSolve): " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, solve.getSessionID());
    sqlite3_bind_text(stmt, 2, solve.getScramble().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, solve.getPenalty());
    sqlite3_bind_double(stmt, 4, solve.getRawTime());
    sqlite3_bind_double(stmt, 5, solve.getFinalTime());
    sqlite3_bind_text(stmt, 6, solve.getDate().c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        std::cerr << "Error while inserting the solve(in saveSolve): " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return;
    }

    solve.setID((int)sqlite3_last_insert_rowid(db));

    sqlite3_finalize(stmt);
    std::cout << "Solve saved succesfully." << std::endl;
    return;
}

void Database::saveSession(sqlite3 *db, const Session &session){
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO sessions (id, event) VALUES (?, ?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if(rc != SQLITE_OK){
        std::cerr << "Error while preparing the consult(in saveSession): " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    sqlite3_bind_int(stmt, 1, session.getSessionID());
    sqlite3_bind_int(stmt, 2, static_cast<int>(session.getEvent()));

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        std::cerr << "Error while inserting the session(in saveSession): " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);
    std::cout << "Session saved succesfully." << std::endl;
    return;
}

// void Database::loadMaxSolveId(sqlite3 *db){
//     int last_id = 0;
//     sqlite3_stmt *stmt = nullptr;
//     const char *sql = "SELECT MAX(id) FROM solves";
//
//     if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK){
//         if(sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_type(stmt, 0) != SQLITE_NULL){ last_id = sqlite3_column_int(stmt, 0); }
//     }
//     max_id = last_id;
//     sqlite3_finalize(stmt);
// }

Session Database::loadSession(sqlite3 *db){
    current_session_id = 0;
    current_solve_amount = 0;
    const char *sql = R"(
        SELECT id, event, (SELECT COUNT(*) FROM solves WHERE session_id = sessions.id)
        FROM sessions 
        ORDER BY id DESC 
        LIMIT 1;
    )";

    sqlite3_stmt *stmt = nullptr;

    if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK){
        std::cerr << "Error SQL(in loadSession: " << sqlite3_errmsg(db) << std::endl;
        return Session();
    }

    Session last_session;

    if(sqlite3_step(stmt) == SQLITE_ROW){
        current_session_id = sqlite3_column_int(stmt, 0);
        int event = sqlite3_column_int(stmt, 1);
        current_solve_amount = sqlite3_column_int(stmt, 2);
        last_session = Session(current_session_id);
        last_session.setEvent(static_cast<Event>(event));
    }
    sqlite3_finalize(stmt);
    return last_session;
}

Session Database::loadSession(sqlite3 *db, int session_to_load){
    const char *sql = R"( SELECT id, event FROM sessions WHERE id = ?; )";

    sqlite3_stmt *stmt = nullptr;

    if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK){
        std::cerr << "Error SQL(in loadSession(id)): " << sqlite3_errmsg(db) << std::endl;
        return Session();
    }

    sqlite3_bind_int(stmt, 1, session_to_load);
    Session s;

    if(sqlite3_step(stmt) == SQLITE_ROW){
        int id = sqlite3_column_int(stmt, 0);
        int event = sqlite3_column_int(stmt, 1);

        s = Session(id);
        s.setEvent(static_cast<Event>(event));
    }
    sqlite3_finalize(stmt);
    return s;
}

std::vector<Solve> Database::loadSolves(sqlite3 *db, int session_to_load){
    std::vector<Solve> v;
    const char *sql = R"( SELECT id, session_id, scramble, penalty, raw_time, final_time, date FROM solves WHERE session_id = ? ORDER BY id ASC; )";
    sqlite3_stmt *stmt = nullptr;

    if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK){
        std::cerr << "Error SQL(in loadSolves): " << sqlite3_errmsg(db) << std::endl;
        return v;
    }

    sqlite3_bind_int(stmt, 1, session_to_load);

    int rc;

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        Solve s;

        s.setID(sqlite3_column_int(stmt, 0));
        s.setSessionID(sqlite3_column_int(stmt, 1));

        const unsigned char *scramble = sqlite3_column_text(stmt, 2);
        s.setScramble(scramble ? reinterpret_cast<const char*>(scramble) : "");

        s.setPenalty(static_cast<Penality>(sqlite3_column_int(stmt, 3)));
        s.setRawTime(sqlite3_column_double(stmt, 4));
        s.setFinalTime(sqlite3_column_double(stmt, 5));

        const unsigned char *date = sqlite3_column_text(stmt, 6);
        s.setDate(date ? reinterpret_cast<const char*>(date) : "");

        v.push_back(s);
    }
    if(rc != SQLITE_DONE){ std::cerr << "Error while loading solves(in loadSolves): " << sqlite3_errmsg(db) << std::endl; }

    sqlite3_finalize(stmt);
    return v;
}

int Database::createSession(sqlite3 *db){
    sqlite3_stmt *stmt = nullptr;
    const char *sql = R"( SELECT MAX(id) FROM sessions; )";
    int last_session = 0;

    if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK){
        if(sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_type(stmt, 0) != SQLITE_NULL){ last_session = sqlite3_column_int(stmt, 0); }
    }
    sqlite3_finalize(stmt);
    current_session_id = last_session + 1;
    return current_session_id;
}

// int Database::initSolve(){ return ++max_id; }
