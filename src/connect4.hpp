#include "gameState.hpp"
#include "cache.hpp"

#include <ctime>

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

class MoveOrdering {
  public:
    // Move ordering
    struct Move {
        int move;
        uint8_t value;
    };
    static void orderMoves(GameState* children, unsigned bestMove, Player player, Move* moves);
    static bool orderMax(const Move& lhs, const Move& rhs) { return lhs.value > rhs.value; }
    static bool orderMin(const Move& lhs, const Move& rhs) { return lhs.value < rhs.value; }

};

class Connect4 {
  public:
    Connect4(long cacheSize) : cache(new Cache(cacheSize)),
                               stats(new Stats()),
                               bufferStart(buffer) {
    }

    ~Connect4() {
        delete cache;
        delete stats;
    }

    Value solve(const GameState& board);

    // Alpha-beta with caching
    Value alphaBeta(const GameState& board, Value alpha, Value beta);
    void principleVariation(const GameState& board, unsigned* moves);


    const Stats& getStats() { return *stats; }
    void resetStats() {
        delete stats;
        stats = new Stats();
    }
    void clearCache() { cache->clear(); }
    void clearCacheValues() { cache->clearValues(); }
    std::string getCacheStats() { return cache->statistics(); }

  private:
    Cache* cache;
    Stats* stats;

    // Pre-allocated state buffer
    GameState buffer[DEPTH_MAX*WIDTH];
    GameState* bufferStart;


    // Optimal line of play

    // No copying allowed
    Connect4(const Connect4& other);
    Connect4& operator=(const Connect4& other);

};
