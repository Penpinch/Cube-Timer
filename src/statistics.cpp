#include "../headers/statistics.hpp"
#include "../headers/statistics.hpp"
#include <vector>
#include <limits>
#include <algorithm>

void Statistic::setBest(float best){ this->best = best; }

void Statistic::setWorst(float worst){ this->worst = worst; }

void Statistic::setAverage(float average){ this->average = average; }

double Statistic::getBest() const{ return best; }

double Statistic::getWorst() const{ return worst; }

double Statistic::getAo5() const{ return ao5; }

double Statistic::getAo12() const{ return ao12; }

double Statistic::getAverage() const{ return average; }

double Statistic::calculateBest(const std::vector<Solve> &solves){
    double best_time = std::numeric_limits<double>::infinity(), 
           time_ = 0.0;

    for(auto it : solves){
        time_ = it.getFinalTime();
        if(time_ < best_time){ best_time = time_; }
    }
    return best_time;
}

double Statistic::calculateWorst(const std::vector<Solve> &solves){
    double worst_time = 0.0, time_ = 0.0;

    for(auto it : solves){
        time_ = it.getFinalTime();
        if(time_ > worst_time){ worst_time = time_; }
    }
    return worst_time;
}

double Statistic::calculateAo5(const std::vector<Solve> &vec_ao5){
    double best_time = calculateBest(vec_ao5),
           worst_time = calculateWorst(vec_ao5),
           sum = 0.0;
    std::vector<double> times;

    for(auto it : vec_ao5){ times.push_back(it.getFinalTime()); }

    times.erase(std::remove(times.begin(), times.end(), best_time), times.end());
    times.erase(std::remove(times.begin(), times.end(), worst_time), times.end());

    for(auto it : times){ sum += it; }
    return sum / 3;
}

double Statistic::calculateAo12(const std::vector<Solve> &vec_ao12){
    double best_time = calculateBest(vec_ao12),
           worst_time = calculateWorst(vec_ao12),
           sum = 0.0;
    std::vector<double> times;

    for(auto it : vec_ao12){ times.push_back(it.getFinalTime()); }

    times.erase(std::remove(times.begin(), times.end(), best_time), times.end());
    times.erase(std::remove(times.begin(), times.end(), worst_time), times.end());

    for(auto it : times){ sum += it; }
    return sum / 10;
}

void Statistic::calculateStats(const std::vector<Solve> &solves){
    double all_time_sum = 0.0;
    int size = solves.size();

    if(size >= 5){
        std::vector<Solve> vec_ao5(solves.end() - 5, solves.end()); 
        ao5 = calculateAo5(vec_ao5);
    } else { ao5 = 0.0; }

    if(size >= 12){
        std::vector<Solve> vec_ao12(solves.end() - 12, solves.end());
        ao12 = calculateAo12(vec_ao12);
    } else { ao12 = 0.0; }

    for(auto it : solves){ all_time_sum += it.getFinalTime(); }
    
    best = calculateBest(solves);
    worst = calculateWorst(solves);
    average = all_time_sum / solves.size();
}

