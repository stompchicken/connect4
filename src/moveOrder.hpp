#ifndef MOVEORDER_H
#define MOVEORDER_H

#include "common.hpp"

/*
 * Reorders children nodes
 */
class MoveOrder {

  public:
    MoveOrder() {
        Move moves[WIDTH] = {3, 2, 4, 1, 5, 0, 6};
        setStaticOrder(moves);
        reset();
    }

    void reset();

    void recordCutoffMove(Depth depth, Move move);

    void setStaticOrder(Move* moves);
    void getStaticOrder(Move* moves) const;

    void reorder(Depth depth, Move* moves) const;
    void reorder(Depth depth, Move bestMove, Move* moves) const;

  private:
    Move staticMoves[WIDTH];
    uint64 staticMoveValue[WIDTH];
    Move killerMove[DEPTH_MAX+1];
};



#endif
