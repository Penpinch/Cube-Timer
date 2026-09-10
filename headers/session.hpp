# ifndef SESSION_HPP
# define SESSION_HPP

# include <string>
# include <vector>
# include "solve.hpp"

class Session {
    private:
        std::string session_name;
        std::vector<Solve> solve_obj;
        int solves_in_session = 0;
    public:
        Session();
        std::string getSessionName() const;
        Solve getSolveInSession(int index) const;
        int getSolvesAmount() const;

        void updateSolvesInSession();
        void addSolve(const Solve& solve);
};

# endif
