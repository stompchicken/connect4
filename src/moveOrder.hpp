#ifndef MOVEORDER_H
#define MOVEORDER_H

#include "common.hpp"
#include "gameState.hpp"

/*
 * Reorders children nodes
 */
class MoveOrder {

  public:
    MoveOrder() {
        reset();
    }

    void cutoff(Depth depth, unsigned move) {
        killerMove[depth] = move;
    }

    void reset() {
        for(unsigned i=0; i<DEPTH_MAX; i++) {
            killerMove[i] = MOVE_INVALID;
        }
    }

    void reorder(Depth depth, unsigned cachedMove, unsigned moves[WIDTH]);

  private:
    uint8 killerMove[DEPTH_MAX];
};



#endif
