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

    Game game(29);
    Connect4 board = Connect4::random(depth);
//    Connect4 board = Connect4(21990500991617, 8796095139072, 1, 12);
    std::cout << board.print() << std::endl;
    std::clock_t start = std::clock();
    Value value = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, maxDepth);
    std::clock_t end = std::clock();
    std::cout << "Value: " << value << std::endl;
    game.printStats((end - start) / (double) CLOCKS_PER_SEC);

    return 0;
}
