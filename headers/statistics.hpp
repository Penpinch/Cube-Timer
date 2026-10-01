#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <vector>
#include "solve.hpp"

class Statistic {
    private:
        double best, worst, average, ao5, ao12;
    public:
        void setBest(float best);
        void setWorst(float worst);
        void setAverage(float average);

        double getBest() const;
        double getWorst() const;
        double getAo5() const;
        double getAo12() const;
        double getAverage() const;

        double calculateBest(const std::vector<Solve> &solves);
        double calculateWorst(const std::vector<Solve> &solves);
        double calculateAo5(const std::vector<Solve> &vec_ao5);
        double calculateAo12(const std::vector<Solve> &vec_ao12);
        void calculateStats(const std::vector<Solve> &solves);
};

#endif
