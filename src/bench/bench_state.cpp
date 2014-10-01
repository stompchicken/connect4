#include "state.hpp"
#include "timer.hpp"

void benchState() {

    int n = 10000;
    {
        Timer timer;
        for(int i=0; i<n; i++) {
            GameState state = GameState::random(32);
            timer.start();
            state.evaluate();
            timer.stop();
        }
        std::cout << "Bitboard.evaluate: ";
        std::cout << timer << std::endl;
    }

    {
        Timer timer;
        GameState buffer[WIDTH];
        for(int i=0; i<n; i++) {
            GameState state = GameState::random(32);
            timer.start();
            state.children(buffer, WIDTH);
            timer.stop();
        }
        std::cout << "Bitboard.children: ";
        std::cout << timer << std::endl;
    }
}
