# include "../headers/session.hpp"
# include <string>

Session::Session(){}

std::string Session::getSessionName() const{ return session_name; }

Solve Session::getSolveInSession(int index) const{ return solve_obj[index]; }

int Session::getSolvesAmount() const{ return solves_in_session; }

void Session::updateSolvesInSession(){ solves_in_session++; }

void Session::addSolve(const Solve& solve){
    solve_obj.push_back(solve);
    updateSolvesInSession();
}
