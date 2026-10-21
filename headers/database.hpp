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
        void saveSolve(sqlite3 *db, const Solve &solve); // 1
        void saveSession(sqlite3 *db, const Session &session); // 2
        void loadMaxSolveId(sqlite3 *db); // 3
        Session loadSession(sqlite3 *db); // 4



        // void saveSolve(const Solve &solve); 1
        std::vector<Solve> loadSolves(int session_to_load);

        // void saveSession(const Session& session); 2
        // Session loadSession(); 4
        Session loadSession(int session_to_load);
        int createSession();
        int initSolve();
        // void loadMaxSolveId(); 3
};

#endif
