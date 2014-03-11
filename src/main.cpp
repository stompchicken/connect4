#include "connect4.hpp"
#include <ctime>

int main(int argc, char* argv[]) {

    if(argc != 2) {
        std::cerr << "Usage: noob [startDepth]" << std::endl;
        exit(-1);
    }

    Depth depth = static_cast<Depth>(std::atoi(argv[1]));

    Connect4 game(20);

    GameState board = GameState::random(depth);
    std::cout << board.print() << std::endl;
    std::clock_t start = std::clock();
    Value value = game.alphaBeta(board, VALUE_MIN, VALUE_MAX);
    std::clock_t end = std::clock();
    std::cout << "Value: " << value << std::endl;
    std::cout << "Duration: " << (end - start) / (double) CLOCKS_PER_SEC << std::endl;
    std::cout << game.getStats() << std::endl;
    game.printCacheStats();
    return 0;
}
