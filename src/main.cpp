# include <iostream>
# include <ostream>
# include "../headers/scrambler.hpp"
# include "../headers/timer.hpp"
# include "../headers/solve.hpp"
# include "../headers/session.hpp"

# include <limits>
#include <string>
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

int main(){
    Session session;
    Scramble scramble;
    Timer timer;
    Timer inspection;
    std::string scblr = "";

    int option = 1;
    while(option != 2){
        std::cout << "--- OPTIONS ---" << std::endl;
        std::cout <<"1. Make a solve." << std::endl;
        std::cout <<"2. Exit." << std::endl;
        std::cout << "Option: "; std::cin >> option;

        if(option == 2){ break; }
        Solve current_solve;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        scblr = scramble.generate3x3Scramble();

        std::cout << scblr << std::endl;
        std::cout << "--- Press ENTER to start the inspection ---";

        std::cin.get();
        inspection.runTimer();

        while(true){
            int elapsed = static_cast<int>(inspection.getElapsedTime());

            std::cout << "\r" << elapsed << "s  " << std::flush;

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
        std::cout << "   SOLVE!!!   " << std::endl;

        while(true){
            double elapsed = timer.getElapsedTime();

            std::cout << "\r" << formatTime(elapsed) << "s  " << std::flush;

            if(keyPressed()){
                consumeKey();
                timer.stopTimer();
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        std::cout << std::endl;

        current_solve.setID(session.getSolvesAmount() + 1);
        current_solve.setScramble(scblr);
        current_solve.setRawTime(timer.getTime());
        current_solve.calculateFinalTime();
        current_solve.calculateDate();

        session.addSolve(current_solve);
    }

    std::cout << std::endl << "SHOW ALL SOLVES" << std::endl;
    for(int i = 0; i < session.getSolvesAmount(); i++){
        Solve obtained_from_sesion = session.getSolveInSession(i);

        std::cout << "ID: " <<  obtained_from_sesion.getID() << std::endl
                  << "Scramble: " << obtained_from_sesion.getScramble() << std::endl
                  << "Time: " << obtained_from_sesion.getFinalTime() << std::endl
                  << "Penality: " << obtained_from_sesion.getPenalty() << std::endl
                  << "Date:" << obtained_from_sesion.getDate() << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
