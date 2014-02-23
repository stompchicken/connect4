#include "gameState.hpp"
#include "cache.hpp"

#define LOG_STATS

struct Stats {
    long nodesExplored;
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

    Value alphaBeta(const GameState& board, Value alpha, Value beta, int maxDepth);

  private:
    Cache* cache;
    Stats* stats;
    GameState buffer[DEPTH_MAX*WIDTH];
    GameState* bufferStart;

    Connect4(const Connect4& other);
    Connect4& operator=(const Connect4& other);

};
