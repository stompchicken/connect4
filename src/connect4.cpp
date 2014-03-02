#include "connect4.hpp"

void orderChildren(GameState* children, int* moves) {
    for(int i=0; i<WIDTH; i++) {
        moves[i] = i;
    }
/*
    moves[0] = 2;
    moves[1] = 3;
    moves[2] = 1;
    moves[3] = 4;
    moves[4] = 0;
    moves[5] = 5;
*/
}


Value Connect4::alphaBeta(const GameState& state, Value alpha, Value beta, int maxDepth) {

#ifdef LOG_STATS
    stats->nodesExplored++;
#endif

    int depth = state.getDepth();
    int player = state.getPlayer();

    // Look up state in cache
    Entry cacheEntry;
    if(cache->get(state, cacheEntry)) {
        return cacheEntry.value;
    }

    Value value = state.evaluate();
    if(value != VALUE_UNKNOWN) {
        return value;
    } else if(depth >= maxDepth) {
        return state.heuristic();
    } else {
        GameState* children = bufferStart;
        state.children(children);
        bufferStart += WIDTH;

        Value a = alpha;
        Value b = beta;
        int bestMove = 0;
        value = (player == PLAYER_MAX) ? VALUE_MIN : VALUE_MAX;

        int moveOrder[WIDTH];
        orderChildren(children, moveOrder);

        for(unsigned i=0; i<WIDTH; i++) {
            unsigned move = moveOrder[i];
            GameState& child = children[move];

            if(!child.isValid()) continue;

            if(player == PLAYER_MAX) {
                Value childVal = alphaBeta(child, a, beta, maxDepth);
                if(childVal > value) {
                    bestMove = move;
                    value = childVal;
                    a = std::max(a, value);
                }
            } else {
                Value childVal = alphaBeta(child, alpha, b, maxDepth);
                if(childVal < value) {
                    bestMove = move;
                    value = childVal;
                    b = std::min(b, value);
                }
            }

            if(a >= b) {
                break;
            }
        }

        cacheEntry.value = value;
        cacheEntry.depth = depth;
        cache->put(state, cacheEntry);

        bufferStart -= WIDTH;

        return value;
    }
}

