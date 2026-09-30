#include "../headers/session.hpp"
#include <vector>

Session::Session(int id): id(id){}

int Session::getSessionID() const{ return id; }

Solve Session::getSolveInSession(int index) const{ return solves[index]; }

std::vector<Solve> Session::getAllSolvesInSession() const{ return solves; }

int Session::getSolvesAmount() const{ return solves_in_session; }

void Session::updateSolvesInSession(){ solves_in_session++; }

void Session::addSolve(const Solve& solve){
    solves.push_back(solve);
    updateSolvesInSession();
}
