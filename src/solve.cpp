#include "../headers/solve.hpp"
#include <string>
#include <chrono>
#include <iostream>

Solve::Solve(){}

void Solve::setSessionID(int sessionID){ this->sessionID = sessionID; }

void Solve::setID(int ID){ this->ID = ID; }

void Solve::setScramble(std::string scramble){ this->scramble = scramble; }

void Solve::setRawTime(double raw_time){ this->raw_time = raw_time; }

void Solve::setPenalty(Penalities penalty){ this->penalty = penalty; }

void Solve::setFinalTime(double final_time){ this->final_time = final_time; }

void Solve::setDate(std::string date){ this->date = date; }

std::string Solve::getScramble() const{ return scramble; }

int Solve::getSessionID() const{ return sessionID; }

int Solve::getID() const{ return ID; }

double Solve::getRawTime() const{ return raw_time; }

double Solve::getFinalTime() const{ return final_time; }

int Solve::getPenalty() const{ return (int)penalty; }

std::string Solve::getDate() const{ return date; }

void Solve::calculateDate(){
    auto current = std::chrono::system_clock::now();

    std::time_t time_ = std::chrono::system_clock::to_time_t(current);
    std::tm* local = std::localtime(&time_);
    
    this->date = std::to_string(local->tm_mday) + "/" + std::to_string(local->tm_mon + 1) + "/" + std::to_string(local->tm_year + 1900);
}

void Solve::calculateFinalTime(){
    if(penalty == Penalities::NORMAL){ final_time = raw_time; }
    else if(penalty == Penalities::PLUSTWO){ final_time = raw_time + 2; }
    else { final_time = -1.0; }
}

void Solve::showSolve() const{
    std::cout << "Session ID: " << getSessionID() << std::endl
              << "ID: " << getID() << std::endl
              << "Scramble: " << getScramble() << std::endl
              << "Time: " << getFinalTime() << std::endl
              << "Penality: " << getPenalty() << std::endl
              << "Date:" << getDate() << std::endl << std::endl;
}
