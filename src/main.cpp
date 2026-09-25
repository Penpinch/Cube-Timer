#include <iostream>
#include <ostream>
#include "../headers/scrambler.hpp"
#include "../headers/timer.hpp"
#include "../headers/solve.hpp"
#include "../headers/session.hpp"
#include "../headers/database.hpp"

#include <limits>
#include <string>
#include <vector>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <thread>

#include <iomanip>
#include <sstream>

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
    database.loadSession();
    int current_session = database.createSession();
    Session session(current_session);
    Scramble scramble;
    Timer timer;
    Timer inspection;
    std::string scblr = "";
    int loaded_solve = database.initSolve();

    int option = 0;
    while(option != 4){
        clearScreen();
        std::cout << "|-------------------------|" << std::endl
                  << "|         OPTIONS         |" << std::endl
                  << "|-------------------------|" << std::endl
                  << "| Make a solve.........(1)|" << std::endl
                  << "| Show solves..........(2)|" << std::endl
                  << "| History..............(3)|" << std::endl
                  << "| Exit.................(4)|" << std::endl
                  << "|-------------------------|" << std::endl;
        std::cout << "- Option: "; std::cin >> option;

        clearScreen();

        if(option == 4){
            if(session.getSolvesAmount() == 0){ break; }
            database.saveSession(session);
            break;
        }
        else if(option < 1 || option > 4){
            std::cout << "|-------------------------------------------------|" << std::endl
                      << "| There's not such option. Press ENTER to continue. |" << std::endl
                      << "|------------------------------------------|" << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            continue;
        }
        else if(option == 2){
            if(session.getSolvesAmount() == 0){
                std::cout << "|-------------------------------------------------|" << std::endl
                          << "| There's no solves yet. Press ENTER to continue. |" << std::endl
                          << "|-------------------------------------------------|" << std::endl;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
                continue;
            }

            std::cout << std::endl << "ALL SOLVES" << std::endl;
            for(int i = 0; i < session.getSolvesAmount(); i++){
                Solve obtained_from_sesion = session.getSolveInSession(i);
                obtained_from_sesion.showSolve();
            }
            std::cout << "|--------------------------|" << std::endl
                      << "| Press ENTER to continue. |" << std::endl
                      << "|--------------------------|" << std::endl;
     
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            continue;
        }
        else if(option == 3){
            int history_option;
            std::cout << "|--------------------------------|" << std::endl
                      << "| Last session history........(1)|" << std::endl
                      << "| Select session history......(2)|" << std::endl
                      << "|--------------------------------|" << std::endl;
            std::cout << "- Option: "; std::cin >> history_option;
            if(history_option < 1 || history_option > 2){ std::cout << "Not such option." << std::endl; continue; }
            
            std::vector<Solve> history;

            if(history_option == 1){ history = database.loadSolves(current_session - 1); }
            else {
                int history_session_id;
                std::cout << "- Session ID: "; std::cin >> history_session_id;
                history = database.loadSolves(history_session_id);
            }

            for(auto it = history.begin(); it != history.end(); ++it){ it->showSolve(); }

            std::cout << "|--------------------------|" << std::endl
                      << "| Press ENTER to continue. |" << std::endl
                      << "|--------------------------|" << std::endl;
    
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            continue;
        }

        Solve current_solve;
        scblr = scramble.generate3x3Scramble();

        std::cout << "|----------------------------------------|" << std::endl
                  << "|   " << scblr << "    |" << std::endl
                  << "|----------------------------------------|" << std::endl
                  << "|  Press ENTER to start the inspection   |" << std::endl
                  << "|----------------------------------------|" << std::endl;
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        inspection.runTimer();
        clearScreen();

        while(true){
            int elapsed = static_cast<int>(inspection.getElapsedTime());

            std::cout << "\r| " << elapsed << " |" << std::flush;

            if(keyPressed()){
                consumeKey();
                inspection.stopTimer();
                break;
            }

            if(elapsed >= 15){
                inspection.stopTimer();
                current_solve.setPenalty(Penalities::DNF);
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        timer.runTimer();
        clearScreen();

        std::cout << "|--------------------------|" << std::endl
                  << "|          SOLVE!!!        |" << std::endl
                  << "|--------------------------|" << std::endl << std::endl;

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
        std::cout << std::endl;

        current_solve.setSessionID(current_session);
        current_solve.setID(loaded_solve);
        current_solve.setScramble(scblr);
        current_solve.setRawTime(timer.getTime());
        current_solve.calculateFinalTime();
        current_solve.calculateDate();

        session.addSolve(current_solve);
        database.saveSolve(current_solve);

        loaded_solve++;
    }

    return 0;
}
