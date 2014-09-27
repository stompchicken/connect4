#include "moveOrder.hpp"

#include <cassert>

void MoveOrder::reset() {
    for(unsigned i=0; i<=DEPTH_MAX; i++) {
        killerMove[i] = MOVE_INVALID;
    }
}

void MoveOrder::recordCutoffMove(Depth depth, Move move) {
#ifdef DEBUG
    assert(depth >= 0 && depth <= DEPTH_MAX);
#endif
    killerMove[depth] = move;
}

void MoveOrder::setStaticOrder(Move* order) {
    for(unsigned i=0; i<WIDTH; i++) {
        staticMoves[i] = order[i];
        staticMoveValue[order[i]] = WIDTH - i;
    }
}

void MoveOrder::getStaticOrder(Move* moves) const {
    for(unsigned i=0; i<WIDTH; i++) {
        moves[i] = staticMoves[i];
    }
}

void MoveOrder::reorder(Depth depth, Move* moves) const {
    reorder(depth, MOVE_INVALID, moves);
}

void MoveOrder::reorder(Depth depth, Move cachedMove, Move* moves) const {
#ifdef DEBUG
    assert(depth >= 0 && depth <= DEPTH_MAX);
#endif

    uint64 value[WIDTH];
    for(Move i=0; i<WIDTH; i++) {
        moves[i] = i;
        value[i] = staticMoveValue[i];

        // Give killer moves priority over static move oerdering
        if(i == killerMove[depth]) { value[i] = 10; }
        // Give cached moves highest priority
        if(i == cachedMove) { value[i] = 11; }
    }

    // Sorting network
    // Generated from http://pages.ripco.net/~jgamble/nw.html
#define XORSWAP(a, b) ((a)^=(b),(b)^=(a),(a)^=(b))
#define SWAP(x, y) if(value[x] < value[y]) { XORSWAP(value[x], value[y]); XORSWAP(moves[x], moves[y]); }
    if(WIDTH == 7) {
        SWAP(1, 2); SWAP(0, 2); SWAP(0, 1); SWAP(3, 4);
        SWAP(5, 6); SWAP(3, 5); SWAP(4, 6); SWAP(4, 5);
        SWAP(0, 4); SWAP(0, 3); SWAP(1, 5); SWAP(2, 6);
        SWAP(2, 5); SWAP(1, 3); SWAP(2, 4); SWAP(2, 3);
    } else if(WIDTH == 6) {
        SWAP(1, 2); SWAP(0, 2); SWAP(0, 1); SWAP(4, 5);
        SWAP(3, 5); SWAP(3, 4); SWAP(0, 3); SWAP(1, 4);
        SWAP(2, 5); SWAP(2, 4); SWAP(1, 3); SWAP(2, 3);
    } else if (WIDTH == 5) {
        SWAP(0, 1); SWAP(3, 4); SWAP(2, 4); SWAP(2, 3);
        SWAP(0, 3); SWAP(0, 2); SWAP(1, 4); SWAP(1, 3);
        SWAP(1, 2);
    } else {
        assert(false && "Width is too small to sort");
    }
}

