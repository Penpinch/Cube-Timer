#include "../headers/database.hpp"
#include "../headers/solve.hpp"
#include "../headers/session.hpp"

#include <sqlite3.h>

#include <fstream>
#include <iostream>
#include <sstream>
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
            date TEXT
            FOREIGN KEY (session_id) REFERENCES session(id)
        );
    )";

    char *error = nullptr;
    if(sqlite3_exec(db, sql, nullptr, nullptr, &error) != SQLITE_OK){
        std::cerr << "Error SQL: " << error << std::endl;
        sqlite3_free(error);
    }
}

void Database::saveSolve(sqlite3 *db, const Solve &solve){
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO solves (id, session_id, scramble, penalty, raw_time, final_time, date) VALUES (?, ?, ?, ?, ?, ?, ?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if(rc != SQLITE_OK){
        std::cerr << "Error while preparing the consult: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, solve.getID()); // cambiar para que lo maneje sql
    sqlite3_bind_int(stmt, 2, solve.getSessionID());
    sqlite3_bind_text(stmt, 3, solve.getScramble().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, solve.getPenalty());
    sqlite3_bind_double(stmt, 5, solve.getRawTime());
    sqlite3_bind_double(stmt, 6, solve.getFinalTime());
    sqlite3_bind_text(stmt, 7, solve.getDate().c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        std::cerr << "Error while inserting the solve: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);
    std::cout << "Solve saved succesfully." << std::endl;
    return;
}

void Database::saveSession(sqlite3 *db, const Session &session){
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO solves (session_id, event) VALUES (?, ?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if(rc != SQLITE_OK){
        std::cerr << "Error while preparing the consult: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    sqlite3_bind_int(stmt, 1, session.getSessionID());
    sqlite3_bind_int(stmt, 2, static_cast<int>(session.getEvent()));

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        std::cerr << "Error while inserting the solve: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);
    std::cout << "Session saved succesfully." << std::endl;
    return;
}

void Database::loadMaxSolveId(sqlite3 *db){
    int last_id = 0;
    sqlite3_stmt *stmt = nullptr;
    const char *sql = "SELECT MAX(id) FROM solves";

    if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK){
        if(sqlite3_step(stmt) == SQLITE_ROW){
            if(sqlite3_column_type(stmt, 0) != SQLITE_NULL){ last_id = sqlite3_column_int(stmt, 0); }
        }
    }
    max_id = last_id;
    sqlite3_finalize(stmt);
}

Session Database::loadSession(sqlite3 *db){
    current_session_id = 0;
    current_solve_amount = 0;
    const char *sql = R"(
        SELECT id, event, (SELECT COUNT(*)) FROM solves WHERE session_id = sessions.id
        FROM sessions 
        ORDER BY id DESC 
        LIMIT 1;
    )";

    sqlite3_stmt *stmt = nullptr;

    if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK){
        std::cerr << "Error SQL: " << sqlite3_errmsg(db) << std::endl;
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







//-------------------------------------------------------------------------------------------------------------------------------------------------------------------





 
std::vector<Solve> Database::loadSolves(int session_to_load){
    std::ifstream file("solves.txt");

    if(!file.is_open()){ std::cerr << "Couldn't open the file." << std::endl; return {}; }

    std::vector<Solve> v;
    std::string line;

    while(std::getline(file, line)){
        std::stringstream ss(line);

        std::string session_id;
        std::getline(ss, session_id, '|');
        if(std::stoi(session_id) != session_to_load){
            session_id.clear();
            continue;
        }

        std::string id, scramble, penalty, raw_time, date;
        std::getline(ss, id, '|');
        std::getline(ss, scramble, '|');
        std::getline(ss, penalty, '|');
        std::getline(ss, raw_time, '|');
        std::getline(ss, date, '|');

        Solve s;
        s.setID(std::stoi(id));
        s.setSessionID(std::stoi(session_id));
        s.setScramble(scramble);
        s.setPenalty(static_cast<Penality>(std::stoi(penalty)));
        s.setRawTime(std::stod(raw_time));
        s.calculateFinalTime(); 
        s.setDate(date);

        v.push_back(s);
    }

    return v;
}

Session Database::loadSession(int session_to_load){
    std::ifstream file("session.txt");
    if(!file.is_open()){ return Session(); }

    int session_id = 0, solves_amount = 0, event = -1;
    char separator;

    while(file >> session_id >> separator >> event >> separator >> solves_amount){
        if(separator != '|'){ std::cerr << "Invalid file format." << std::endl; return Session(); }

        if(session_id == session_to_load){
            Session s(session_id);
            s.setEvent(static_cast<Event>(event));

            return s;
        }
    }

    return Session();
}

int Database::createSession(){ loadMaxSolveId(); return ++current_session_id; }

int Database::initSolve(){ return ++max_id; }
