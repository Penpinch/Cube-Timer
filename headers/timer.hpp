# ifndef TIMER_HPP
# define TIMER_HPP

# include <chrono>

class Timer {
    private:
        std::chrono::steady_clock::time_point start_time;
        double time;
    public:
        Timer();
        void runTimer();
        void stopTimer();
        double getTime() const;
};

# endif
