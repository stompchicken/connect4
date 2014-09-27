#include "gameState.hpp"
#include "cache.hpp"
#include "moveOrder.hpp"

#include "pool.hpp"

#include <ctime>

struct Stats {
    Stats() {
        reset();
    }

    void reset() {
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

struct Config {

    Config() : useCache(true),
               maxCacheDepth(DEPTH_MAX - 8),
               reorderMoves(true),
               depthFirstScan(true) {
    };

    bool useCache;
    Depth maxCacheDepth;
    bool reorderMoves; 
    bool depthFirstScan;
};
std::ostream& operator<<(std::ostream &output, const Config &config);


class Connect4 {
  public:
    Connect4(long cacheSize) : width(7), height(6),
                               cache(new Cache(cacheSize)),
                               stats(new Stats()),
                               statePool(WIDTH*(DEPTH_MAX+1)),
                               moveOrder(), config() {
    }

    ~Connect4() {
        delete cache;
        delete stats;
    }

    void setBoardSize(unsigned width, unsigned height) {
        this->width = width;
        this->height = height;
    }

    Value solve(const GameState& board);
    void printState(const GameState& state, const std::string& msg, Value alphaBeta, Value alpha, Value beta);

    Value minimax(const GameState& board);

    // Alpha-beta with caching
    Value alphaBeta(const GameState& board, const Value alpha, const Value beta, Move& bestMove);
    void principleVariation(const GameState& board, Move buffer[DEPTH_MAX]);

    void reset() {
        cache->clear();
        stats->reset();
        moveOrder.reset();
    }

    const Stats& getStats() const { return *stats; }
    std::string getCacheStats() const { return cache->statistics(); }

  private:
    unsigned width;
    unsigned height;
    Cache* cache;
    Stats* stats;
    Pool<GameState> statePool;
    MoveOrder moveOrder;
    Config config;

    // No copying allowed
    Connect4(const Connect4& other);
    Connect4& operator=(const Connect4& other);
};
