# include "../headers/timer.hpp"
# include <chrono>
using namespace std;

Timer::Timer(): time{0.0}{}

void Timer::runTimer(){ start_time = chrono::steady_clock::now(); }

void Timer::stopTimer(){
    auto end_time = chrono::steady_clock::now();
    time = chrono::duration<double>(end_time - start_time).count();
}

double Timer::getTime() const{ return time; }
