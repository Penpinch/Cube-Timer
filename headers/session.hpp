#ifndef SESSION_HPP
#define SESSION_HPP

#include <vector>
#include <string>
#include "solve.hpp"
#include "event.hpp"

class Session {
    private:
        Event event;
        int id = 0;
        std::string date;
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
        std::string getDate() const;

        void setEvent(Event event);
        void setDate(std::string date);

        void updateSolvesInSession();
        void addSolve(const Solve& solve);
        void calculateDate();
};

#endif
