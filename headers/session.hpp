#ifndef SESSION_HPP
#define SESSION_HPP

#include <vector>
#include "solve.hpp"
#include "event.hpp"

class Session {
    private:
        Event event;
        int id = 0;
        std::vector<Solve> solves;
        int solves_in_session = 0;
    public:
        Session();
        Session(int id);
        int getSessionID() const;
        Solve getSolveInSession(int index) const;
        std::vector<Solve> getAllSolvesInSession() const;
        int getSolvesAmount() const;
        Event getEvent() const;

        void setEvent(Event event);

        void updateSolvesInSession();
        void addSolve(const Solve& solve);
};

#endif
