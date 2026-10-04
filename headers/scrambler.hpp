#ifndef SCRAMBLER_HPP
#define SCRAMBLER_HPP

#include <vector>
#include <string>
#include <random>
#include "event.hpp"

enum class SpecialCases { NORMAL = 0, PRIME = 1, DOUBLE = 2 };

enum class MoveType { NORMAL = 0, WIDE = 1 };

enum class MaxMoves { C2X2 = 11, C3X3 = 20, C4X4 = 40 };

class Scramble {
    private:
        std::random_device rd;
        std::mt19937 gen;

        std::vector<char> cube_sides = {'L', 'R', 'F', 'B', 'U', 'D'};
    public:
        Scramble();
        std::string generateMove(Event event);
        // std::string generate2x2Scramble();
        // std::string generate3x3Scramble();
        // std::string generate4x4Scramble();

        std::string generate(Event event);
};

#endif
