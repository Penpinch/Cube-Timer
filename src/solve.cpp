#include "../headers/solve.hpp"
#include <iomanip>
#include <string>
#include <iostream>
#include <ios>

Solve::Solve(){
    session_id = 0;
    id = 0;
    raw_time = 0;
    final_time = 0;
    penalty = Penality::NORMAL;
}

void Solve::setSessionID(int session_id){ this->session_id = session_id; }

void Solve::setID(int id){ this->id = id; }

void Solve::setScramble(std::string scramble){ this->scramble = scramble; }

void Solve::setRawTime(double raw_time){ this->raw_time = raw_time; }

void Solve::setPenalty(Penality penalty){ this->penalty = penalty; }

void Solve::setFinalTime(double final_time){ this->final_time = final_time; }

void Solve::setDate(std::string date){ this->date = date; }

std::string Solve::getScramble() const{ return scramble; }

int Solve::getSessionID() const{ return session_id; }

int Solve::getID() const{ return id; }

double Solve::getRawTime() const{ return raw_time; }

double Solve::getFinalTime() const{ return final_time; }

int Solve::getPenalty() const{ return (int)penalty; }

std::string Solve::getDate() const{ return date; }

void Solve::calculateFinalTime(){
    if(penalty == Penality::NORMAL){ final_time = raw_time; }
    else if(penalty == Penality::PLUSTWO){ final_time = raw_time + 2; }
    else { final_time = -1.0; }
}

void Solve::showSolve() const{
    std::cout << "Session ID: " << getSessionID() << '\n'
              << "ID: " << getID() << '\n'
              << "Scramble: " << getScramble() << '\n';

    if(getFinalTime() != -1){ 
        std::streamsize original_precision = std::cout.precision();

        std::cout << "Time: " << std::fixed << std::setprecision(2) << getFinalTime() << '\n'; 

        std::cout << std::setprecision(original_precision);
        std::cout.unsetf(std::ios_base::floatfield);
    }
    else { std::cout << "Time: " << "DNF\n"; }

    std::cout << "Penality: " << getPenalty() << '\n'
              << "Date:" << getDate() << "\n\n";
}
