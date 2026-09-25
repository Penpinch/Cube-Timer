# include "../headers/database.hpp"
# include "../headers/solve.hpp"
# include <fstream>
# include <iostream>
# include <sstream>
# include <string>
# include <vector>

int Database::getCurrentSessionId() const{ return current_session_Id; }

int Database::getCurrentSolvesAmount() const{ return current_solve_amount; }

void Database::saveSolve(const Solve &solve){
    std::ofstream file("solves.txt", std::ios::app);

    if(!file.is_open()){ std::cerr << "Could't open the file." << std::endl; return; }

    file << solve.getSessionID() << '|' << solve.getID() << '|' << solve.getScramble() << '|' << solve.getPenalty() << '|'
         << solve.getFinalTime() << '|' << solve.getDate() << "\n";
}
 
std::vector<Solve> Database::loadSolves(int session_to_load){
    std::ifstream file("solves.txt");

    if(!file.is_open()){ std::cerr << "Couldn't open the file." << std::endl; return {}; }

    std::vector<Solve> v;
    std::string line;

    while(std::getline(file, line)){
        std::stringstream ss(line);

        std::string session_id;
        std::getline(ss, session_id, '|');
        if(std::stoi(session_id) != session_to_load){
            session_id.clear();
            continue;
        }

        std::string id, scramble, penalty, final_time, date;
        std::getline(ss, id, '|');
        std::getline(ss, scramble, '|');
        std::getline(ss, penalty, '|');
        std::getline(ss, final_time, '|');
        std::getline(ss, date, '|');

        Solve s;
        s.setID(std::stoi(id));
        s.setSessionID(std::stoi(session_id));
        s.setScramble(scramble);
        s.setPenalty(static_cast<Penalities>(std::stoi(penalty)));
        s.setFinalTime(std::stoi(final_time));
        s.setDate(date);

        v.push_back(s);
    }

    return v;
}

void Database::saveSession(const Session& session){
    std::ofstream file("session.txt", std::ios::app);

    if(!file.is_open()){ std::cerr << "Couldn't open the file." << std::endl; return; }

    file << session.getSessionID() << '|' << session.getSolvesAmount() << '\n';
}

void Database::loadSession(){
    std::ifstream file("session.txt");

    current_session_Id = 0;
    current_solve_amount = 0;

    if(!file.is_open()){ return; }

    int session_id = 0, solves_amount = 0;
    char separator;

    while(file >> session_id >> separator >> solves_amount){
        if(separator != '|'){ std::cerr << "Invalid file format." << std::endl; return; }

        if(session_id > current_session_Id){ 
            current_session_Id = session_id;
            current_solve_amount = solves_amount;
        }
    }
}

int Database::createSession(){ return ++current_session_Id; }

int Database::initSolve(){ return ++current_solve_amount; }
