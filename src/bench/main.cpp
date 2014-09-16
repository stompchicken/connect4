#include <iostream>

#include "timer.hpp"
#include "state.hpp"

int main();
void benchState();

int main() {

    std::cout << "Benchmarking state..." << std::endl;
    benchState();
}

void benchState() {

    int n = 100000;

    {
        Timer timer;
        for(int i=0; i<n; i++) {
            NaiveState state;
            state.random(4);

            timer.start();
            state.evaluate();
            timer.stop();
        }
        std::cout << "NaiveState.evaluate, ";
        std::cout << timer << std::endl;
    }

    {
        Timer timer;
        NaiveState buffer[WIDTH];
        for(int i=0; i<n; i++) {
            NaiveState state;
            state.random(4);

            timer.start();
            state.children(buffer, WIDTH);
            timer.stop();
        }
        std::cout << "NaiveState.children, ";
        std::cout << timer << std::endl;
    }

    {
        Timer timer;
        for(int i=0; i<n; i++) {
            GameState state;
            state.random(4);

            timer.start();
            state.evaluate();
            timer.stop();
        }
        std::cout << "Bitboard.evaluate, ";
        std::cout << timer << std::endl;
    }

    {
        Timer timer;
        GameState buffer[WIDTH];
        for(int i=0; i<n; i++) {
            GameState state;
            state.random(4);

            timer.start();
            state.children(buffer, WIDTH);
            timer.stop();
        }
        std::cout << "Bitboard.children, ";
        std::cout << timer << std::endl;
    }
}
