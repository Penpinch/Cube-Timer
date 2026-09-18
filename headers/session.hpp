# ifndef SESSION_HPP
# define SESSION_HPP

# include <vector>
# include "solve.hpp"

class Session {
    private:
        int id = 0;
        std::vector<Solve> solves;
        int solves_in_session = 0;
    public:
        Session();
        Session(int id);
        int getSessionID() const;
        Solve getSolveInSession(int index) const;
        int getSolvesAmount() const;

        void updateSolvesInSession();
        void addSolve(const Solve& solve);
};

# endif
