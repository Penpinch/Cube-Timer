#include "../headers/statistics.hpp"
#include "../headers/statistics.hpp"
#include <vector>
#include <limits>

void Statistic::setBest(float best){ this->best = best; }

void Statistic::setWorst(float worst){ this->worst = worst; }

void Statistic::setAverage(float average){ this->average = average; }

double Statistic::getBest() const{ return best; }

double Statistic::getWorst() const{ return worst; }

double Statistic::getAverage() const{ return average; }

void Statistic::calculateStats(const std::vector<Solve> &solves){
    double sum = 0.0, worst_time = 0.0, best_time = std::numeric_limits<double>::infinity();
    
    for(auto it : solves){
        double time_ = it.getFinalTime();
        if(time_ < best_time){ best_time = time_; }
        if(time_ > worst_time){ worst_time = time_; }
        sum += time_; 
    }
    best = best_time;
    worst = worst_time;
    average = sum / solves.size();
}

