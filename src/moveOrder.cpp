#include "moveOrder.hpp"

//uint8 staticMoveOrder[WIDTH] = {7, 6, 5, 4, 3, 2};

uint8 staticMoveOrder[WIDTH] = {2, 4, 6, 7, 5, 3};

void MoveOrder::reorder(Depth depth, unsigned cachedMove, unsigned moves[WIDTH]) {
    uint64 value[WIDTH];
    for(unsigned i=0; i<WIDTH; i++) {
        moves[i] = i;
        value[i] = staticMoveOrder[i];

        if(i == killerMove[depth]) { value[i] = 10; }

        if(i == cachedMove) { value[i] = 11; }
    }

    // Sorting network
    unsigned tempMove;
    unsigned tempValue;
#define SWAP(x, y) if(value[x] < value[y]) { tempMove = moves[y]; tempValue = value[y]; moves[y] = moves[x]; value[y] = value[x]; moves[x] = tempMove; value[x] = tempValue; }
#if WIDTH == 7
    SWAP(1, 2); SWAP(0, 2); SWAP(0, 1); SWAP(3, 4);
    SWAP(5, 6); SWAP(3, 5); SWAP(4, 6); SWAP(4, 5);
    SWAP(0, 4); SWAP(0, 3); SWAP(1, 5); SWAP(2, 6);
    SWAP(2, 5); SWAP(1, 3); SWAP(2, 4); SWAP(2, 3);
#else
    SWAP(1, 2); SWAP(0, 2); SWAP(0, 1); SWAP(4, 5);
    SWAP(3, 5); SWAP(3, 4); SWAP(0, 3); SWAP(1, 4);
    SWAP(2, 5); SWAP(2, 4); SWAP(1, 3); SWAP(2, 3);
#endif
}
