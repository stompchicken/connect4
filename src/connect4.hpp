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
    MoveOrdering() {
        for(int depth=0; depth<DEPTH_MAX; depth++) {
            killerMove[depth] = MOVE_INVALID;

            for(unsigned i=0; i<WIDTH; i++) {
                moves[depth*i].move = 0;
                moves[depth*i].value = 0;
            }

        }
    }

    // Move ordering
    struct Move {
        unsigned move;
        unsigned value;
    };

    void orderMoves(const GameState& parent, GameState* children, unsigned bestMove);
    Move* getMoves(Depth depth) { return moves + (depth * WIDTH); }


    Move moves[WIDTH*DEPTH_MAX];
    unsigned killerMove[DEPTH_MAX];

};

std::ostream& operator<<(std::ostream &output, const MoveOrdering &moveOrdering);


class Connect4 {
  public:
    Connect4(long cacheSize) : cache(new Cache(cacheSize)),
                               stats(new Stats()),
                               moveTop(0) {
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

    Moves movePool[DEPTH_MAX];
    unsigned moveTop;

    // Optimal line of play


    // No copying allowed
    Connect4(const Connect4& other);
    Connect4& operator=(const Connect4& other);

};
