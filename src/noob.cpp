#include <iostream>
#include <stdint.h>
#include <ctime>

#include "connect4.hpp"
#include "cache.hpp"
#include "game.hpp"

int main(int argc, char* argv[]) {

    if(argc != 4) {
        std::cerr << "Usage: noob [startDepth] [endDepth] [seed]" << std::endl;
        exit(-1);
    }

    int depth = std::atoi(argv[1]);
    int maxDepth = std::atoi(argv[2]);
    int seed = std::atoi(argv[3]);

    srand(seed);

    Game game(24);
    Connect4 board = Connect4::random(depth);
    std::cout << board.print() << std::endl;
    Value value = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, maxDepth);
    std::cout << "Value: " << value << std::endl;

    return 0;
}
