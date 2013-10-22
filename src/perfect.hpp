#ifndef PERFECT
#define PERFECT

#include "game.hpp"
#include "cache.hpp"

struct Stats {
    long nodesExplored;
    long cacheHits;
    long cacheMisses;
    long cutoffs;
    long terminalNodes;
    bool complete;

    Stats() : nodesExplored(0), cacheHits(0), cacheMisses(0),
              cutoffs(0), terminalNodes(0), complete(false) {}

    friend std::ostream& operator<<(std::ostream&, const Stats&);
};

class Game {
  public:
    Game(int hashBits) : cache(new Cache(hashBits)), stats(new Stats()), bufferStart(buffer) {
    }

    ~Game() {
        delete cache;
        delete stats;
    }

    Value alphaBeta(const Connect4& board, Value alpha, Value beta, int maxDepth);
    void reset() {
        cache->resetBounds();
        bufferStart = buffer;
    }

  private:
    Cache* cache;
    Stats* stats;
    Connect4 buffer[DEPTH_MAX*WIDTH];
    Connect4* bufferStart;

    Game(const Game& other);
    Game& operator=(const Game& other);

};


class NodeOrdering {

  public:
    NodeOrdering() {
        for(unsigned i=0; i<WIDTH; i++) {
            ordering[i].index = i;
            ordering[i].score = 0;
        }
    }

    void increment(int index, int increment) {
        for(unsigned i=0; i<WIDTH; i++) {
            if(ordering[i].index == index) {
                ordering[i].score += increment;

                // BUBBLE SORT IS THE BEST SORT
                for(;i > 0 && ordering[i].score > ordering[i-1].score; i--) {
                    std::swap(ordering[i], ordering[i-1]);
                }

                break;
            }
        }
    }

    int move(int i) const {
        return ordering[i].index;
    }

  private:
    struct NodeScore {
        int index;
        uint64 score;
    };
    NodeScore ordering[WIDTH];
};

#endif
