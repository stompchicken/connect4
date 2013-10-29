#include <iostream>
#include <ctime>
#include <vector>

#include "connect4.hpp"
#include "cache.hpp"
#include "game.hpp"


void benchmark_pruning() {
    srand(8086);

    std::clock_t bench_start = std::clock();
    for(size_t i=0; i<1000; i++) {
        Game game(24);
        Connect4 board = Connect4::random(12);
        std::cout << i << std::endl;
        std::cout << board.print() << std::endl;
        std::clock_t start = std::clock();
        Value value = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, 42);
        std::clock_t end = std::clock();
        double duration = (end - start) / (double) CLOCKS_PER_SEC;
        std::cout << value << ", " << board.heuristic() << std::endl;
//        std::cout << duration << "/";
//        std::cout << (end - bench_start) / (double) CLOCKS_PER_SEC << std::endl;
    }
    std::cout << std::endl;
}

