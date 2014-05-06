#include "gameState.hpp"
#include "cache.hpp"
#include "moveOrder.hpp"

#include "pool.hpp"

#include <ctime>

struct Stats {
    Stats() {
        nodesExplored = 0;
        cutoffs = 0;
        averageCutoff = 0.0;
        cutoffTotal = 0;
        cacheHits = 0;
        cacheMisses = 0;
    }

    uint64 nodesExplored;
    uint64 cutoffs;
    double averageCutoff;
    uint64 cutoffTotal;
    uint64 cacheHits;
    uint64 cacheMisses;
};

std::ostream& operator<<(std::ostream &output, const Stats &stats);

class Connect4 {
  public:
    Connect4(long cacheSize) : cache(new Cache(cacheSize)),
                               stats(new Stats()),
                               statePool(WIDTH*(DEPTH_MAX+1)) {
    }

    ~Connect4() {
        delete cache;
        delete stats;
    }

    Value solve(const GameState& board);

    // Alpha-beta with caching
    Value alphaBeta(const GameState& board, Value alpha, Value beta);
    void principleVariation(const GameState& board, unsigned* moves);


    const Stats& getStats() const { return *stats; }
    void resetStats() {
        delete stats;
        stats = new Stats();
    }
    void clearCache() { cache->clear(); }
    void clearCacheValues() { cache->clearValues(); }
    std::string getCacheStats() const { return cache->statistics(); }

  private:
    Cache* cache;
    Stats* stats;

    Pool<GameState> statePool;

    MoveOrder moveOrder;

    // No copying allowed
    Connect4(const Connect4& other);
    Connect4& operator=(const Connect4& other);

};
