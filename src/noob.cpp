#include <iostream>
#include <stdint.h>
#include <thread>
#include <functional>
#include <ctime>

#include "connect4.hpp"
#include "cache.hpp"
#include "perfect.hpp"

void printStats(Stats& stats, Cache& cache, std::clock_t start) {

    int seconds = 0;
    while(stats.complete == false) {
        double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;

        if(duration > seconds + 1) {
            std::cout << "Time elapsed: " << seconds << "s" << std::endl;
            std::cout << stats << std::endl;

            float kNodesPerSecond = static_cast<float>(stats.nodesExplored) / static_cast<float>(seconds*1000);
            std::cout << kNodesPerSecond << "\tKnodes/s" << std::endl;

            std::cout << cache.statistics() << std::endl;
            seconds = static_cast<int>(duration);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

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
    Connect4 board = genBoard(depth);
    std::cout << board.print() << std::endl;
    Value value = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, maxDepth);
    std::cout << "Value: " << value << std::endl;




//    std::thread statsThread(printStats, std::ref(stats), std::ref(cache), std::clock());
/*
    for(int i=depth+1; i<maxDepth; i+=2) {
        std::clock_t start = std::clock();
        std::cout << "Solving to depth " << i << std::endl;
        Value value = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, i);
        std::cout << "Resetting..." << std::endl;
        game.reset();
        double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
        std::cout << "Finished in " << duration << " seconds" << std::endl;
        std::cout << "Value: " << value << std::endl;
    }
*/
//    stats.complete = true;
//    statsThread.join();

    return 0;
}
