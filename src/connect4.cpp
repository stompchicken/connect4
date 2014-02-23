#include "connect4.hpp"

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

        for(unsigned i=0; i<WIDTH; i++) {
            unsigned move = i;
            GameState& child = children[i];

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

