# ifndef DATABASE_HPP
# define DATABASE_HPP

# include "session.hpp"
# include "solve.hpp"
# include <vector>

class Database {
    private:
        int current_session_Id;
        int current_solve_amount;
    public:
        int getCurrentSessionId() const;
        int getCurrentSolvesAmount() const;

        void saveSolve(const Solve& solve);
        std::vector<Solve> loadSolves();

        void saveSession(const Session& session);
        void loadSession();
        int createSession();
        int initSolve();
};

# endif
