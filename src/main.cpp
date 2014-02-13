#include "connect4.hpp"
#include <ctime>

int main(int argc, char* argv[]) {

    if(argc != 4) {
        std::cerr << "Usage: noob [startDepth] [endDepth] [seed]" << std::endl;
        exit(-1);
    }

    int depth = std::atoi(argv[1]);
    int maxDepth = std::atoi(argv[2]);
    int seed = std::atoi(argv[3]);

    srand(seed);

    Connect4 game(28);
    GameState board = GameState::random(depth);
    std::cout << board.print() << std::endl;
    std::clock_t start = std::clock();
    Value value = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, maxDepth);
    std::clock_t end = std::clock();
    std::cout << "Value: " << value << std::endl;
    game.printStats((end - start) / (double) CLOCKS_PER_SEC);

    return 0;
}
