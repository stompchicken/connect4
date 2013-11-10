#include <iostream>
#include <ctime>
#include <vector>

#include "connect4.hpp"
#include "cache.hpp"
#include "game.hpp"


void benchmark_heuristic() {
    srand(8086);

    for(size_t i=0; i<1000; i++) {
        Game game(24);
        Connect4 board = Connect4::random(12);
        std::cout << i << std::endl;
        std::cout << board.print() << std::endl;
        Value value = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, 42);
        std::cout << value << ", " << board.heuristic() << std::endl;
    }
    std::cout << std::endl;
}

