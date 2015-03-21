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

    Config() : width(7),
               height(6),
               useCache(true),
               cacheSize(1*Cache::Megabytes),
               cacheProbe(8),
               cacheMaxDepth(DEPTH_MAX),
               cacheReplacement(true),
               cachePrefetch(false),
               reorderMoves(true),
               moveStatic(true),
               moveKiller(true),
               moveBest(true),
               preFilter(true) {
    }

    unsigned width;
    unsigned height;

    bool useCache;
    uint64 cacheSize;
    unsigned cacheProbe;
    Depth cacheMaxDepth;
    bool cacheReplacement;
    bool cachePrefetch;

    bool reorderMoves;
    bool moveStatic;
    bool moveKiller;
    bool moveBest;

    bool preFilter;
};
std::ostream& operator<<(std::ostream &output, const Config &config);


class Connect4 {
  public:
    Connect4(Config conf) : config(conf),
                            cache(new Cache(conf.cacheSize, conf.cacheProbe, conf.cacheReplacement)),
                            stats(new Stats()),
                            statePool(WIDTH*(DEPTH_MAX+1)),
                            moveOrder(conf.moveStatic, conf.moveKiller, conf.moveBest)  {
    }

    ~Connect4() {
        delete cache;
        delete stats;
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
    Config config;
    Cache* cache;
    Stats* stats;
    Pool<GameState> statePool;
    MoveOrder moveOrder;

    // No copying allowed
    Connect4(const Connect4& other);
    Connect4& operator=(const Connect4& other);
};
