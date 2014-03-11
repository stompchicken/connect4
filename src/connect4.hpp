#include "gameState.hpp"
#include "cache.hpp"

struct Stats {
    Stats() {
        nodesExplored = 0;
        cutoffs = 0;
        cacheHits = 0;
        cacheMisses = 0;
    }

    long nodesExplored;
    long cutoffs;
    long cacheHits;
    long cacheMisses;
};

std::ostream& operator<<(std::ostream &output, const Stats &stats);

class Connect4 {
  public:
    Connect4(int hashBits) : cache(new Cache(hashBits)), stats(new Stats()), bufferStart(buffer) {
    }

    ~Connect4() {
        delete cache;
        delete stats;
    }

    // Alpha-beta with caching
    Value alphaBeta(const GameState& board, Value alpha, Value beta);

    const Stats& getStats() { return *stats; }
    void resetStats() {
        delete stats;
        stats = new Stats();
    }
    void clearCache() { cache->clear(); }
    void printCacheStats() { std::cout << cache->statistics() << std::endl; }

  private:
    Cache* cache;
    Stats* stats;

    // Pre-allocated state buffer
    GameState buffer[DEPTH_MAX*WIDTH];
    GameState* bufferStart;

    // Mover ordering
    unsigned moves[WIDTH];
    void orderChildren(GameState* children);

    // Optimal line of play
    unsigned principleVariation[DEPTH_MAX];

    Connect4(const Connect4& other);
    Connect4& operator=(const Connect4& other);

};
