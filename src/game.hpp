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
    NodeOrdering(const Connect4* nodes, Player player) {
        for(unsigned i=0; i<WIDTH; i++) {
            indexes[i] = i;
            values[i] = nodes[i].isValid() ? nodes[i].heuristic() : VALUE_MIN;
        }

        for(uint8 i=1; i<WIDTH; i++) {

            uint8 tmpIdx = indexes[i];
            uint8 tmpVal = values[i];

            uint8 j = i;
            while(j > 0 && (player == PLAYER_MAX) ? tmpVal > values[j-1] : tmpVal < values[j-1]) {
                values[j] = values[j-1];
                indexes[j] = indexes[j-1];
                j--;
            }
            indexes[j] = tmpIdx;
            values[j] = tmpVal;
        }
    }

    uint move(uint i) const {
        return indexes[i];
    }

    friend std::ostream& operator<< (std::ostream& o, NodeOrdering const& fred);

  private:
    uint8 indexes[WIDTH];
    uint8 values[WIDTH];
};




#endif
