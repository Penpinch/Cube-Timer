#include "../headers/session.hpp"
#include <string>
#include <vector>
#include <chrono>

Session::Session(){ calculateDate(); }

Session::Session(int id): id(id){ calculateDate(); }

int Session::getSessionID() const{ return id; }

Solve Session::getSolveInSession(int index) const{ return solves[index]; }

std::vector<Solve> Session::getAllSolvesInSession() const{ return solves; }

int Session::getSolvesAmount() const{ return solves_in_session; }

Event Session::getEvent() const{ return event; }

std::string Session::getDate() const{ return date; }

void Session::setEvent(Event event){ this->event = event; }

void Session::setDate(std::string date){ this->date = date; }

void Session::updateSolvesInSession(){ solves_in_session++; }

void Session::addSolve(const Solve& solve){
    solves.push_back(solve);
    updateSolvesInSession();
}

void Session::calculateDate(){
    auto current = std::chrono::system_clock::now();

    std::time_t time_ = std::chrono::system_clock::to_time_t(current);
    std::tm* local = std::localtime(&time_);
    
    this->date = std::to_string(local->tm_mday) + "/" + std::to_string(local->tm_mon + 1) + "/" + std::to_string(local->tm_year + 1900);
}
