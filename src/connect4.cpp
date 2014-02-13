#include "connect4.hpp"

#include <ctime>

Value Connect4::alphaBeta(const GameState& state, Value alpha, Value beta, int maxDepth) {

//    stats->nodesExplored++;

    int depth = state.getDepth();
    int player = state.getPlayer();

    std::clock_t start;
    if(depth <= 8) {
        start = std::clock();
    }

    // Look up state in cache
    CacheValue cacheValue;
    if(cache->get(state, cacheValue)) {
//        stats->cacheHits++;

        if(depth <= 8) {
//            stats->printNode(depth, start, std::clock(), true);
        }

        return cacheValue.value;
    } else {
//        stats->cacheMisses++;
    }

    Value value = state.evaluate();
    if(value != VALUE_UNKNOWN) {
//        stats->terminalNodes += 1;
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

        for(unsigned i=0; i<WIDTH; i++) {
            int move = i;
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
//                stats->cutoffs += 1;
                break;
            }
        }

        cacheValue.value = value;
        cacheValue.depth = depth;
        cache->put(state, cacheValue);

        bufferStart -= WIDTH;

        if(depth <= 8) {
//            stats->printNode(depth, start, std::clock(), false);
        }

        return value;
    }
}

