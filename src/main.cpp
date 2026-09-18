# include <iostream>
# include <ostream>
# include "../headers/scrambler.hpp"
# include "../headers/timer.hpp"
# include "../headers/solve.hpp"
# include "../headers/session.hpp"
# include "../headers/database.hpp"

# include <limits>
# include <string>
# include <unistd.h>
# include <termios.h>
# include <fcntl.h>
# include <thread>

# include <iomanip>
# include <sstream>

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
    while(option != 3){
        clearScreen();
        std::cout << "|-------------------------|" << std::endl
                  << "|         OPTIONS         |" << std::endl
                  << "|-------------------------|" << std::endl
                  << "| Make a solve.........(1)|" << std::endl
                  << "| Show solves..........(2)|" << std::endl
                  << "| Exit.................(3)|" << std::endl
                  << "|-------------------------|" << std::endl;
        std::cout << "- Option: "; std::cin >> option;

        clearScreen();

        if(option == 3){
            // for(int i = 0; i < session.getSolvesAmount(); i++){ database.saveSolve(session.getSolveInSession(i)); }
            database.saveSession(session);
            break;
        }
        else if(option < 1 || option > 3){
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

                std::cout << "ID: " <<  obtained_from_sesion.getID() << std::endl
                          << "Scramble: " << obtained_from_sesion.getScramble() << std::endl
                          << "Time: " << obtained_from_sesion.getFinalTime() << std::endl
                          << "Penality: " << obtained_from_sesion.getPenalty() << std::endl
                          << "Date:" << obtained_from_sesion.getDate() << std::endl << std::endl;
            }
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
