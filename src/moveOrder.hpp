#ifndef MOVEORDER_H
#define MOVEORDER_H

#include "common.hpp"

/*
 * Reorders children nodes
 */
class MoveOrder {

  public:
    MoveOrder(bool staticMove, bool killerMove, bool bestMove) {
        Move moves[WIDTH] = {3, 2, 4, 1, 5, 0, 6};
        setStaticOrder(moves);
        useStaticMove = staticMove;
        useKillerMove = killerMove;
        useBestMove = bestMove;
        reset();
    }

    void reset();

    void recordCutoffMove(Depth depth, Move move);

    void setStaticOrder(Move* moves);
    void getStaticOrder(Move* moves) const;

    void reorder(Depth depth, Move* moves) const;
    void reorder(Depth depth, Move bestMove, Move* moves) const;

  private:
    bool useStaticMove;
    Move staticMoves[WIDTH];
    uint64 staticMoveValue[WIDTH];

    bool useKillerMove;
    Move killerMove[DEPTH_MAX+1];

    bool useBestMove;
};



#endif
