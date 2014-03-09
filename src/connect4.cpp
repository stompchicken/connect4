#include "connect4.hpp"

std::ostream& operator<<(std::ostream &output, const Stats &stats) {
    output << stats.nodesExplored << " nodes explored" << std::endl;
    output << stats.cacheHits << " cache hits" << std::endl;
    output << stats.cacheMisses << " cache misses" << std::endl;
    return output;
}


void Connect4::orderChildren(GameState* ) {
    for(unsigned i=0; i<WIDTH; i++) {
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


Value Connect4::alphaBeta(const GameState& state, Value alpha, Value beta, unsigned* pv) {
    stats->nodesExplored++;

    Depth depth = state.getDepth();
    Player player = state.getPlayer();

    if(depth > 100) {
        for(int i=18; i<depth; i++) { std::cout << " "; }
        std::cout << static_cast<int>(depth) << std::endl;
        std::cout << printPlayer(player) << std::endl;
        std::cout << state.print() << std::endl;
    }


#ifdef DEBUG
    assert(depth < DEPTH_MAX);
#endif

    // Look up state in cache
    Entry cacheEntry;

    if(cache->get(state, cacheEntry)) {
        stats->cacheHits++;
        principleVariation[depth] = cacheEntry.bestMove;
        return cacheEntry.value;
    } else {
        stats->cacheMisses++;
    }

    Value value = state.evaluate();
    if(value != VALUE_UNKNOWN) {
        return value;
    } else {
        GameState* children = bufferStart;
        state.children(children);
        bufferStart += WIDTH;

        Value a = alpha;
        Value b = beta;
        unsigned bestMove;
        unsigned variation[DEPTH_MAX];

        orderChildren(children);
        for(unsigned i=0; i<WIDTH; i++) {
            unsigned move = moves[i];
            GameState& child = children[move];

            if(!child.isValid()) continue;

            Value childVal;
            if(player == PLAYER_MAX) {
                childVal = alphaBeta(child, a, beta, variation);
                if(value == VALUE_UNKNOWN || childVal > value) {
                    value = childVal;
                    bestMove = move;
                    a = std::max(a, value);

                    for(unsigned i=depth+1; i<DEPTH_MAX; i++) {
                        pv[i] = variation[i];
                    }

                }
            } else {
                childVal = alphaBeta(child, alpha, b, variation);
                if(value == VALUE_UNKNOWN || childVal < value) {
                    bestMove = move;
                    value = childVal;
                    b = std::min(b, value);

                    for(unsigned i=depth+1; i<DEPTH_MAX; i++) {
                        pv[i] = variation[i];
                    }

                }
            }

            if(a > b) {
                break;
            }
        }

        cacheEntry.value = value;
        cacheEntry.depth = depth;
        cacheEntry.bestMove = bestMove;
        cache->put(state, cacheEntry);

        pv[depth] = bestMove;


        bufferStart -= WIDTH;

        return value;
    }
}

