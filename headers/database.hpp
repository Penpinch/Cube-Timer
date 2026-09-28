# ifndef DATABASE_HPP
# define DATABASE_HPP

# include "session.hpp"
# include "solve.hpp"
# include <vector>

class Database {
    private:
        int current_session_id;
        int current_solve_amount;
        int max_id;
    public:
        int getCurrentSessionId() const;
        int getCurrentSolvesAmount() const;
        int getMaxId() const;

        void saveSolve(const Solve& solve);
        std::vector<Solve> loadSolves(int session_to_load);

        void saveSession(const Session& session);
        void loadSession();
        int createSession();
        int initSolve();
        void loadMaxSolveId();
};

# endif
