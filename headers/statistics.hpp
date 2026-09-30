#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <vector>
#include "solve.hpp"

class Statistic {
    private:
        double best, worst, average;
    public:
        void setBest(float best);
        void setWorst(float worst);
        void setAverage(float average);

        double getBest() const;
        double getWorst() const;
        double getAverage() const;

        void calculateStats(const std::vector<Solve> &solves);
};

#endif
