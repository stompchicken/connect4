#include "gameState.hpp"
#include "cache.hpp"

#define ENABLE_STATS

struct Stats {
    Stats() {
        nodesExplored = 0;
        cacheHits = 0;
        cacheMisses = 0;
    }

    long nodesExplored;
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

    Value alphaBeta(const GameState& board, Value alpha, Value beta, unsigned* pv);
    const Stats& getStats() { return *stats; }
    void resetStats() {
        delete stats;
        stats = new Stats();
    }
    void clearCache() { cache->clear(); }

//  private:
    Cache* cache;
    Stats* stats;
    GameState buffer[DEPTH_MAX*WIDTH];
    GameState* bufferStart;

    unsigned moves[WIDTH];
    void orderChildren(GameState* children);

    unsigned principleVariation[DEPTH_MAX];

    Connect4(const Connect4& other);
    Connect4& operator=(const Connect4& other);

};
