#ifndef GAME
#define GAME

#include "noob.hpp"
#include "connect4.hpp"
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

    void printStats(double duration) {
        std::cout << duration << "\t seconds" << std::endl;
        std::cout << stats->nodesExplored / duration << "\t nodes per seconds" << std::endl;
        std::cout << *stats << std::endl;
    }

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
    NodeOrdering(Connect4* nodes, Player player) {
        for(unsigned i=0; i<WIDTH; i++) {
            ordering[i].index = i;
            ordering[i].score = nodes[i].isValid() ? nodes[i].heuristic() : VALUE_MIN;
        }

        if(player == PLAYER_MAX) {
            std::sort(ordering, ordering+WIDTH, compareMax);
        } else if(player == PLAYER_MIN) {
            std::sort(ordering, ordering+WIDTH, compareMin);
        } else {
            assert(false);
        }

    }

    int move(int i) const {
        return ordering[i].index;
    }

    friend std::ostream& operator<< (std::ostream& o, NodeOrdering const& fred);

  private:
    struct NodeScore {
        uint index;
        uint score;
    };

    static bool compareMax(NodeScore n1, NodeScore n2) {
        return (n1.score > n2.score);
    }

    static bool compareMin(NodeScore n1, NodeScore n2) {
        return (n1.score < n2.score);
    }

    NodeScore ordering[WIDTH];
};




#endif
