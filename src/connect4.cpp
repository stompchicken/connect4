#include "connect4.hpp"

std::ostream& operator<<(std::ostream &output, const Stats &stats) {
    output << stats.nodesExplored << " nodes explored" << std::endl;
    output << stats.cutoffs << " cutoffs" << std::endl;

    float hitRate = float(stats.cacheHits)/float(stats.cacheHits+stats.cacheMisses);
    output << hitRate << " cache hitrate (" << stats.cacheHits << "/" << stats.cacheHits+stats.cacheMisses << ")" << std::endl;

    return output;
}


void Connect4::orderChildren(GameState* ) {
/*    for(unsigned i=0; i<WIDTH; i++) {
        moves[i] = i;
    }
*/

    moves[0] = 2;
    moves[1] = 3;
    moves[2] = 1;
    moves[3] = 4;
    moves[4] = 0;
    moves[5] = 5;
}

Value Connect4::alphaBeta(const GameState& state, Value alpha, Value beta) {
    stats->nodesExplored++;

    Depth depth = state.getDepth();
    Player player = state.getPlayer();

#ifdef DEBUG
    assert(state.isValid());
    assert(depth < DEPTH_MAX);
    assert(alpha <= beta);
#endif

    // Look up state in cache
    Entry cacheEntry;
    if(cache->get(state, cacheEntry)) {
        stats->cacheHits++;

        if(cacheEntry.lower == cacheEntry.upper) {
            return cacheEntry.lower;
        } else if(cacheEntry.lower >= beta) {
            return cacheEntry.lower;
        } else if(cacheEntry.upper <= alpha) {
            return cacheEntry.upper;
        }

        alpha = std::max(alpha, cacheEntry.lower);
        beta = std::min(beta, cacheEntry.upper);
    } else {
        stats->cacheMisses++;

        cacheEntry.lower = VALUE_MIN;
        cacheEntry.upper = VALUE_MAX;

    }

    Value value = state.evaluate();
    if(value != VALUE_UNKNOWN) {
        // Don't bother caching, evaluate is fast
        return value;
    } else {
        GameState* children = bufferStart;
        state.children(children);
        bufferStart += WIDTH;

        Value a = alpha;
        Value b = beta;

        orderChildren(children);
        for(unsigned i=0; i<WIDTH; i++) {
            unsigned move = moves[i];
            GameState& child = children[move];

            if(!child.isValid()) continue;

            Value childVal;
            if(player == PLAYER_MAX) {
                childVal = alphaBeta(child, a, beta);
                if(value == VALUE_UNKNOWN || childVal > value) {
                    value = childVal;
                    a = std::max(a, value);
                }
            } else {
                childVal = alphaBeta(child, alpha, b);
                if(value == VALUE_UNKNOWN || childVal < value) {
                    value = childVal;
                    b = std::min(b, value);
                }
            }

            if(a >= b) {
                stats->cutoffs++;
                break;
            }
        }

        // Store new bounds to cache
        if(value <= alpha) {
            cacheEntry.upper = value;
        } else if (value > alpha && value < beta) {
            cacheEntry.lower = value;
            cacheEntry.upper = value;
        } else if(value >= beta) {
            cacheEntry.lower = value;
        }
        cacheEntry.depth = depth;
        cache->put(state, cacheEntry);

        bufferStart -= WIDTH;
        return value;
    }
}

