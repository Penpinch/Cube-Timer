# ifndef SCRAMBLER_HPP
# define SCRAMBLER_HPP

# include <vector>
# include <string>
# include <random>

enum class SpecialCases {
    NORMAL = 0, PRIME = 1, DOUBLE = 2
};

class Scramble {
    private:
        int max_moves_per_scramble = 12;
        std::random_device rd;
        std::mt19937 gen;

        std::vector<char> cube_sides = {'L', 'R', 'F', 'B', 'U', 'D'};
    public:
        Scramble();
        std::string generate3x3Scramble();
};

# endif
