#include "connect4.hpp"

#include <algorithm>
#include <math.h>

#define MAX_CACHE_DEPTH WIDTH*HEIGHT - 2

Value Connect4::solve(const GameState& state) {
    moveOrder.reset();
    Value value = alphaBeta(state, VALUE_MIN, VALUE_MAX);
    return value;
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

//    cache->prefetch(state);
    Value value = state.evaluate();
    if(value != VALUE_UNKNOWN) {
        // Don't bother caching, evaluate is fast
        return value;
    } else {
        // Look up state in cache
        Entry cacheEntry;
        unsigned bestMove = MOVE_INVALID;
        if(depth <= MAX_CACHE_DEPTH && cache->get(state, cacheEntry)) {
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

            if(cacheEntry.bestMove != MOVE_INVALID) {
                bestMove = cacheEntry.bestMove;
            }
        } else {
            stats->cacheMisses++;
            cacheEntry.lower = VALUE_MIN;
            cacheEntry.upper = VALUE_MAX;
        }

        unsigned moves[WIDTH];
        PoolEntry<GameState> children = statePool.get(WIDTH);

        moveOrder.reorder(depth+1, MOVE_INVALID, moves);
        state.children(children.data);

        Value a = alpha;
        Value b = beta;
        for(unsigned i=0; i<WIDTH; i++) {
            const unsigned move = moves[i];
            const GameState& child = children.data[move];

            if(!child.isValid()) continue;

            Value childVal;
            if(player == PLAYER_MAX) {
                childVal = alphaBeta(child, a, beta);
                if(value == VALUE_UNKNOWN || childVal > value) {
                    value = childVal;
                    bestMove = move;
                    moveOrder.cutoff(depth+1, bestMove);
                    a = std::max(a, value);
                }
            } else {
                childVal = alphaBeta(child, alpha, b);
                if(value == VALUE_UNKNOWN || childVal < value) {
                    value = childVal;
                    bestMove = move;
                    moveOrder.cutoff(depth+1, bestMove);
                    b = std::min(b, value);
                }
            }

            if(a >= b) {
                stats->cutoffs++;
                stats->cutoffTotal++;
                stats->averageCutoff += (i - stats->averageCutoff) / stats->cutoffTotal;
                break;
            }
        }

        // Store new bounds to cache
        if(depth <= MAX_CACHE_DEPTH) {
            if(value <= alpha) {
                cacheEntry.upper = value;
                cacheEntry.bestMove = MOVE_INVALID;
            } else if (value > alpha && value < beta) {
                cacheEntry.lower = value;
                cacheEntry.upper = value;
                cacheEntry.bestMove = bestMove;
            } else if(value >= beta) {
                cacheEntry.lower = value;
                cacheEntry.bestMove = MOVE_INVALID;
            }

            cacheEntry.depth = depth;
            cache->put(state, cacheEntry);
        }
        return value;
    }
}

void Connect4::principleVariation(const GameState& state, unsigned* moves) {
    GameState board = state;
    GameState children[WIDTH];
    Entry cacheEntry;
    while(cache->get(board, cacheEntry)) {
        assert(cacheEntry.lower <= cacheEntry.upper);

//        std::cout << board.print() << std::endl;
        std::cout << static_cast<int>(cacheEntry.bestMove) << std::endl;
//        std::cout << printValue(cacheEntry.lower) << std::endl;
//        std::cout << std::endl;
        board.children(children);
        board = children[cacheEntry.bestMove];

        moves[0] = cacheEntry.bestMove;
        assert(board.isValid());
    }
}
