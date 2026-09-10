# ifndef SOLVE_HPP
# define SOLVE_HPP

# include <string>

enum class Penalities{
    NORMAL = 0,
    DNF = 1,
    PLUSTWO = 2
};

class Solve {
    private:
        int ID;
        std::string scramble;
        double raw_time, final_time;
        Penalities penalty;
        std::string date;
    public:
        Solve(std::string scramble, double raw_time);

        // Setters.
        void setID(int ID);
        void setPenalty(Penalities penalty);

        // Getters.
        std::string getScramble() const;
        int getID() const;
        double getRawTime() const;
        double getFinalTime() const;
        int getPenalty() const;
        std::string getDate() const;

        void calculateDate();
        void calculateFinalTime();
};


# endif
