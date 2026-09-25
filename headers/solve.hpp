# ifndef SOLVE_HPP
# define SOLVE_HPP

# include <string>

enum class Penalities {
    NORMAL = 0,
    DNF = 1,
    PLUSTWO = 2
};

class Solve {
    private:
        int sessionID;
        int ID;
        std::string scramble;
        double raw_time, final_time;
        Penalities penalty;
        std::string date;
    public:
        Solve();

        // Setters.
        void setSessionID(int sessionID);
        void setID(int ID);
        void setScramble(std::string);
        void setRawTime(double raw_time);
        void setPenalty(Penalities penalty);
        void setFinalTime(double final_time);
        void setDate(std::string date);

        // Getters.
        std::string getScramble() const;
        int getSessionID() const;
        int getID() const;
        double getRawTime() const;
        double getFinalTime() const;
        int getPenalty() const;
        std::string getDate() const;

        void calculateDate();
        void calculateFinalTime();

        void showSolve() const;
};


# endif
