#include "connect4.hpp"
#include <ctime>
#include <math.h>

int main(int argc, char* argv[]) {

    if(argc != 2) {
        std::cerr << "Usage: noob [startDepth]" << std::endl;
        exit(-1);
    }

    Depth depth = static_cast<Depth>(std::atoi(argv[1]));

    long size = pow(2, 24);
    Connect4 game(size);

    GameState board = GameState::random(depth);
    std::cout << board.print() << std::endl;
    std::clock_t start = std::clock();
    Value value = game.alphaBeta(board, VALUE_MIN, VALUE_MAX);
    std::clock_t end = std::clock();
    std::cout << "Value: " << printValue(value) << std::endl;
    std::cout << "Duration: " << (end - start) / (double) CLOCKS_PER_SEC << std::endl;
    std::cout << game.getStats();
    game.printCacheStats();
/*

    game.clearCacheValues();
    std::cout << "Clearing cache" << std::endl;
    std::cout << board.print() << std::endl;
    start = std::clock();
    value = game.alphaBeta(board, VALUE_MIN, VALUE_MAX);
    end = std::clock();
    std::cout << "Value: " << printValue(value) << std::endl;
    std::cout << "Duration: " << (end - start) / (double) CLOCKS_PER_SEC << std::endl;
    std::cout << game.getStats();
    game.printCacheStats();
*/

    return 0;
}
