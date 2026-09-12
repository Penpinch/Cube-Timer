# include "../headers/timer.hpp"
# include <chrono>

Timer::Timer(): time(0.0){}

void Timer::runTimer(){ start_time = std::chrono::steady_clock::now(); }

void Timer::stopTimer(){
    auto end_time = std::chrono::steady_clock::now();
    time = std::chrono::duration<double>(end_time - start_time).count();
}

double Timer::getTime() const{ return time; }

double Timer::getElapsedTime() const{
    auto now = std::chrono::steady_clock::now();

    return std::chrono::duration<double>(now - start_time).count();
}
