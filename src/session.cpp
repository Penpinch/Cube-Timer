# include "../headers/session.hpp"

Session::Session(int id): id(id){}

int Session::getSessionID() const{ return id; }

Solve Session::getSolveInSession(int index) const{ return solves[index]; }

int Session::getSolvesAmount() const{ return solves_in_session; }

void Session::updateSolvesInSession(){ solves_in_session++; }

void Session::addSolve(const Solve& solve){
    solves.push_back(solve);
    updateSolvesInSession();
}
