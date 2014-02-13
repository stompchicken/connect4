#include "gameState.hpp"
#include "cache.hpp"

class Stats {};

class Connect4 {
  public:
    Connect4(int hashBits) : cache(new Cache(hashBits)), stats(new Stats()), bufferStart(buffer) {
    }

    ~Connect4() {
        delete cache;
        delete stats;
    }

    Value alphaBeta(const GameState& board, Value alpha, Value beta, int maxDepth);

    void printStats(double) {
/*
        std::cout << duration << "\t seconds" << std::endl;
        std::cout << stats->nodesExplored / duration << "\t nodes per seconds" << std::endl;
        std::cout << *stats << std::endl;

        std::cout << cache->statistics() << std::endl;
*/
    }

  private:
    Cache* cache;
    Stats* stats;
    GameState buffer[DEPTH_MAX*WIDTH];
    GameState* bufferStart;

    Connect4(const Connect4& other);
    Connect4& operator=(const Connect4& other);

};
