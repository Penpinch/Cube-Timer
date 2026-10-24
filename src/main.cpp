#include <ios>
#include <iostream>
#include <ostream>
#include "../headers/scrambler.hpp"
#include "../headers/timer.hpp"
#include "../headers/solve.hpp"
#include "../headers/session.hpp"
#include "../headers/database.hpp"
#include "../headers/statistics.hpp"

#include <limits>
#include <string>
#include <vector>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <thread>

#include <iomanip>
#include <sstream>

// g++ main.cpp -o programa -lsqlite3

bool keyPressed(){
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    int ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF){ ungetc(ch, stdin); return true; }

    return false;
}

void consumeKey(){ getchar(); }

std::string formatTime(double seconds){
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << seconds;

    return oss.str();
}

void clearScreen(){ std::cout << "\033[2J\033[1;1H"; }

int main(){
    Database database;
    sqlite3 *db = database.openDatabase();
    database.createTables(db);
    Statistic stats;
    Session loaded = database.loadSession(db);
    int current_session = database.createSession(db);
    Session session(current_session);
    Scramble scramble;
    Timer timer;
    Timer inspection;
    std::string scblr = "";

    int puzzle_option;
    std::cout << "|-------------------------------|\n"
              << "|            PUZZLES            |\n"
              << "|-------------------------------|\n"
              << "| 2x2........................(1)|\n"
              << "| 3x3........................(2)|\n"
              << "| 4x4........................(3)|\n"
              << "|-------------------------------|\n";
    std::cout << "- Option: "; std::cin >> puzzle_option;

    Event event;
    switch(puzzle_option){
        case 1: { event = Event::CUBE_2X2; break; }
        case 2: { event = Event::CUBE_3X3; break; }
        case 3: { event = Event::CUBE_4X4; break; }
        default: { std::cerr << "There is no such a puzzle\n"; return 1; }
    }
    session.setEvent(event);
    database.saveSession(db, session);

    int option = 0;
    while(option != 6){
        clearScreen();
        std::cout << "|-------------------------------|\n"
                  << "|            OPTIONS            |\n"
                  << "|-------------------------------|\n"
                  << "| Make a solve...............(1)|\n"
                  << "| Show solves................(2)|\n"
                  << "| History....................(3)|\n"
                  << "| Statistics.................(4)|\n"
                  << "| Change puzzle..............(5)|\n"
                  << "| Exit.......................(6)|\n"
                  << "|-------------------------------|\n";
        std::cout << "- Option: "; std::cin >> option;

        clearScreen();

        if(option < 1 || option > 6){
            std::cout << "|---------------------------------------------------|\n"
                      << "| There's not such option. Press ENTER to continue. |\n"
                      << "|---------------------------------------------------|\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            continue;
        }
        else if(option == 6){
            sqlite3_exec(db, "DELETE FROM sessions WHERE id NOT IN (SELECT DISTINCT session_id FROM solves)", nullptr, nullptr, nullptr);
            database.closeDatabase(db);
            break;
        }
        else if(option == 2){
            if(session.getSolvesAmount() == 0){
                std::cout << "|-------------------------------------------------|\n"
                          << "| There's no solves yet. Press ENTER to continue. |\n"
                          << "|-------------------------------------------------|\n";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
                continue;
            }

            std::cout << "|--------------------|\n"
                      << "|     ALL SOLVES     |\n"
                      << "|--------------------|\n";

            for(int i = 0; i < session.getSolvesAmount(); i++){
                Solve obtained_from_sesion = session.getSolveInSession(i);
                obtained_from_sesion.showSolve();
            }
            std::cout << "|--------------------------|\n"
                      << "| Press ENTER to continue. |\n"
                      << "|--------------------------|\n";
     
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            continue;
        }
        else if(option == 3){
            if(current_session == 1){
                std::cout << "|-----------------------------------------------------|\n"
                          << "| No other sessions to load. Press ENTER to continue. |\n"
                          << "|-----------------------------------------------------|\n";
    
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
                continue;
            }

            int history_option;
            std::cout << "|--------------------------------|\n"
                      << "| Last session history........(1)|\n"
                      << "| Select session history......(2)|\n"
                      << "|--------------------------------|\n";
            std::cout << "- Option: "; std::cin >> history_option;
            if(history_option < 1 || history_option > 2){ std::cout << "Not such option.\n"; continue; }

            std::vector<Solve> history;

            if(history_option == 1){ history = database.loadSolves(db,current_session - 1); loaded = database.loadSession(db); }
            else {
                int history_session_id;
                std::cout << "- Session ID: "; std::cin >> history_session_id;
                history = database.loadSolves(db, history_session_id);
                loaded = database.loadSession(db, history_session_id);
            }

            std::string pzzl;
            switch(loaded.getEvent()){
                case Event::CUBE_2X2: { pzzl = "2x2"; break; }
                case Event::CUBE_3X3: { pzzl = "3x3"; break; }
                case Event::CUBE_4X4: { pzzl = "4x4"; break; }
            }

            std::cout << "|--------------------------|\n"
                      << "|        PUZZLE: " << pzzl << "       |\n"
                      << "|--------------------------|\n";

            for(auto it = history.begin(); it != history.end(); ++it){ it->showSolve(); }

            std::cout << "|--------------------------|\n"
                      << "| Press ENTER to continue. |\n"
                      << "|--------------------------|\n";
    
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            continue;
        }
        else if(option == 4){
            int stats_option = 0;
            if(current_session == 1){ 
                std::cout << "|----------------------------|\n"
                          << "| Current session stats      |\n"
                          << "|----------------------------|\n";
                stats_option = 1; 
            } else {
                std::cout << "|----------------------------------|\n"
                          << "| Current session stats.........(1)|\n"
                          << "| Last session stats............(2)|\n"
                          << "| Select session history........(3)|\n"
                          << "|----------------------------------|\n";
                std::cout << "- Option: "; std::cin >> stats_option;
                if(stats_option < 1 || stats_option > 3){ std::cout << "Not such option.\n"; continue; }
            }

            std::vector<Solve> stats_loaded;

            if(stats_option == 1){ stats_loaded = session.getAllSolvesInSession(); }
            else if(stats_option == 2){ stats_loaded = database.loadSolves(db, current_session - 1); }
            else {
                int stats_session_id;
                std::cout << "- Session ID: "; std::cin >> stats_session_id;
                stats_loaded = database.loadSolves(db, stats_session_id);
            }

            stats.calculateStats(stats_loaded);
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Best: " << stats.getBest() << '\n'
                      << "Worst: " << stats.getWorst() << '\n'
                      << "Ao5:" << stats.getAo5() << '\n'
                      << "Ao12:" << stats.getAo12() << '\n'
                      << "Average: " << stats.getAverage() << '\n';

            std::cout << "|--------------------------|\n"
                      << "| Press ENTER to continue. |\n"
                      << "|--------------------------|\n";
    
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            continue;
        }
        else if(option == 5){
            int new_puzzle_option;

            std::cout << "|-------------------------------|\n"
                      << "|            PUZZLES            |\n"
                      << "|-------------------------------|\n"
                      << "| 2x2........................(1)|\n"
                      << "| 3x3........................(2)|\n"
                      << "| 4x4........................(3)|\n"
                      << "|-------------------------------|\n";
            std::cout << "- Option: "; std::cin >> new_puzzle_option;

            if(puzzle_option < 1 || puzzle_option > 3){ std::cout << "There is not such a puzzle.\n"; continue; }
            if(puzzle_option == new_puzzle_option){ continue; }
            puzzle_option = new_puzzle_option;

            current_session = database.createSession(db);
            session = Session(current_session);

            switch(puzzle_option){
                case 1: { session.setEvent(Event::CUBE_2X2); break; }
                case 2: { session.setEvent(Event::CUBE_3X3); break; }
                case 3: { session.setEvent(Event::CUBE_4X4); break; }
                default: { std::cout << "There is not such a puzzle.\n"; continue; }
            }
            database.saveSession(db, session);
            continue;
        }
       
        Solve current_solve;
        scblr = scramble.generate(session.getEvent());

        std::cout << "|----------------------------------------|\n"
                  << "|   " << scblr << "    |\n" // cada 8 movimientos salta linea
                  << "|----------------------------------------|\n"
                  << "|  Press ENTER to start the inspection   |\n"
                  << "|----------------------------------------|\n";
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        inspection.runTimer();
        clearScreen();

        std::cout << "|----------------------------------|\n"
                  << "|            INSPECTION            |\n"
                  << "|----------------------------------|\n\n";

        while(true){
            double elapsed = static_cast<int>(inspection.getElapsedTime());

            if(elapsed >= 15){ std::cout << "\r| " << "+2" << " |" << std::flush; }
            else { std::cout << "\r| " << elapsed << " |" << std::flush; }

            if(keyPressed()){
                consumeKey();
                inspection.stopTimer();
                break;
            }

            if(elapsed >= 17){
                inspection.stopTimer();
                current_solve.setPenalty(Penality::DNF);
                break;
            }

            if(elapsed >= 15){ current_solve.setPenalty(Penality::PLUSTWO); }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        if(static_cast<Penality>(current_solve.getPenalty()) != Penality::DNF){
            timer.runTimer();
            clearScreen();

            std::cout << "|----------------------------------|\n"
                      << "|              SOLVE!!!            |\n"
                      << "|----------------------------------|\n\n";

            while(true){
                double elapsed = timer.getElapsedTime();

                std::cout << "\r| " << formatTime(elapsed) << " |" << std::flush;

                if(keyPressed()){
                    consumeKey();
                    timer.stopTimer();
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            std::cout << '\n';
        }

        current_solve.setSessionID(current_session);
        current_solve.setScramble(scblr);
        if(static_cast<Penality>(current_solve.getPenalty()) == Penality::DNF){ current_solve.setRawTime(0.0); }
        else { current_solve.setRawTime(timer.getTime()); }
        current_solve.calculateFinalTime();
        current_solve.setDate(session.getDate());

        database.saveSolve(db, current_solve);
        session.addSolve(current_solve);
    }

    return 0;
}
