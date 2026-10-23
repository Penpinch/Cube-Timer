#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "session.hpp"
#include "solve.hpp"
#include <sqlite3.h>

class Database {
    private:
        int current_session_id;
        int current_solve_amount;
        int max_id;
    public:
        int getCurrentSessionId() const;
        int getCurrentSolvesAmount() const;
        int getMaxId() const;

        sqlite3 *openDatabase();
        void closeDatabase(sqlite3 *db);
        void createTables(sqlite3 *db);
        void saveSolve(sqlite3 *db, Solve &solve);
        void saveSession(sqlite3 *db, const Session &session);
        // void loadMaxSolveId(sqlite3 *db);
        Session loadSession(sqlite3 *db);
        Session loadSession(sqlite3 *db, int session_to_load);
        std::vector<Solve> loadSolves(sqlite3 *db, int session_to_load);
        int createSession(sqlite3 *db);
        // int initSolve();
};

#endif
