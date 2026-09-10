# include "../headers/scrambler.hpp"

Scramble::Scramble() : rd(), gen(rd()){}

std::string Scramble::generate3x3Scramble(){ 
    std::uniform_int_distribution<> distrib(0, 5);
    std::uniform_int_distribution<> dist(0, 2);

    std::string scmbr = "";
    int last_num = -1;

    for(int i = 0; i < max_moves_per_scramble; i++){
        int num = distrib(this->gen);
        if(last_num == num){ i--; continue; }

        scmbr += this->cube_sides[num];

        SpecialCases sc = static_cast<SpecialCases>(dist(this->gen));
        if(sc == SpecialCases::PRIME){ scmbr += "'"; }
        else if(sc == SpecialCases::DOUBLE){scmbr += "2"; }

        scmbr += " ";
        last_num = num;
    }

    return scmbr;
}
