#include "../headers/scrambler.hpp"
#include <string>

Scramble::Scramble() : rd(), gen(rd()){}

std::string Scramble::generateMove(Event event){
    std::uniform_int_distribution<> faces(0, 5);
    std::uniform_int_distribution<> special(0, 2);
    std::uniform_int_distribution<> type(0, 1);

    std::string scmbr = "";
    int last_num = -1;
    int moves = 0;

    switch (event){
        case Event::CUBE_2X2: { moves = static_cast<int>(MaxMoves::C2X2); break; }
        case Event::CUBE_3X3: { moves = static_cast<int>(MaxMoves::C3X3); break; }
        case Event::CUBE_4X4: { moves = static_cast<int>(MaxMoves::C4X4); break; }
    }

    for(int i = 0; i < moves; ){
        int num = faces(gen);

        if(last_num == num){ continue; }

        scmbr += cube_sides[num];

        if(event == Event::CUBE_4X4){
            MoveType mt = static_cast<MoveType>(type(gen));

            if(mt == MoveType::WIDE){ scmbr += "w"; }
        }

        SpecialCases sc = static_cast<SpecialCases>(special(gen));

        if(sc == SpecialCases::PRIME){ scmbr += "'"; }
        else if(sc == SpecialCases::DOUBLE){ scmbr += "2"; }

        scmbr += " ";
        last_num = num;
        i++;
    }
    return scmbr;
}

std::string Scramble::generate(Event event){
    switch(event){
        case Event::CUBE_2X2: return generateMove(Event::CUBE_2X2);
        case Event::CUBE_3X3: return generateMove(Event::CUBE_3X3);
        case Event::CUBE_4X4: return generateMove(Event::CUBE_4X4);
    }
    return "";
}
