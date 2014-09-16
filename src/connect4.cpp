#include "connect4.hpp"

#include <math.h>

#define MAX_CACHE_DEPTH WIDTH*HEIGHT - 4

Value Connect4::solve(const GameState& state) {
    this->reset();
    moveOrder.reset();
    Move bestMove;
    Value value = alphaBeta(state, VALUE_MIN, VALUE_MAX, bestMove);
    return value;
}

Value Connect4::alphaBeta(const GameState& state, const Value alpha, const Value beta, Move& bestMove) {
    stats->nodesExplored++;

    Depth depth = state.getDepth();
    Player player = state.getPlayer();



#ifdef DEBUG
    assert(state.isValid());
    assert(depth <= DEPTH_MAX);
    assert(alpha <= beta);
    bestMove = MOVE_INVALID;
#endif

//    cache->prefetch(state);

    Value value = state.evaluate();
    if(value != VALUE_UNKNOWN) {
        return value;
    } else if(depth == this->height * this->width) {
        return VALUE_DRAW;
    }

    Value a = alpha;
    Value b = beta;

    // Look up state in cache
    Entry cacheEntry;
    if(depth <= maxCacheDepth && cache->get(state, cacheEntry)) {
        if(depth <= 33) { std::cout << "Cache hit" << std::endl; }
        stats->cacheHits++;

        if(cacheEntry.lower == cacheEntry.upper) {
            return cacheEntry.lower;
        } else if(cacheEntry.lower >= beta) {
            return cacheEntry.lower;
        } else if(cacheEntry.upper <= alpha) {
            return cacheEntry.upper;
        }

        a = std::max(a, cacheEntry.lower);
        b = std::min(b, cacheEntry.upper);

    } else {
        stats->cacheMisses++;
        cacheEntry.lower = VALUE_MIN;
        cacheEntry.upper = VALUE_MAX;
    }

    // Generate children and reorder
    unsigned moves[WIDTH];
    PoolEntry<GameState> children = statePool.get(WIDTH);
    moveOrder.reorder(depth+1, MOVE_INVALID, moves);
    state.children(children.data, this->width, this->height);
/*
    // Check for obvious winners
    for(Move move=0; move<WIDTH; move++) {
        const GameState& child = children.data[move];
        if(!child.isValid()) continue;
        Value value = child.evaluate();
        if(player == PLAYER_MAX && value == VALUE_MAX) {
            bestMove = move;
            return VALUE_MAX;
        }
        if(player == PLAYER_MIN && value == VALUE_MIN) {
            bestMove = move;
            return VALUE_MIN;
        }
    }
*/
    for(unsigned i=0; i<WIDTH; i++) {
        const Move move = moves[i];
        const GameState& child = children.data[move];

        if(!child.isValid()) continue;

        Value childVal;
        Move childBestMove;
        if(player == PLAYER_MAX) {
            childVal = alphaBeta(child, a, beta, childBestMove);

            if(value == VALUE_UNKNOWN || childVal > value) {
                value = childVal;
                bestMove = move;
                moveOrder.recordCutoffMove(depth+1, bestMove);
                a = std::max(a, value);
            }
        } else {
            childVal = alphaBeta(child, alpha, b, childBestMove);
            if(value == VALUE_UNKNOWN || childVal < value) {
                value = childVal;
                bestMove = move;
                moveOrder.recordCutoffMove(depth+1, bestMove);
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
    if(depth <= maxCacheDepth) {

        if (value >= alpha && value <= beta) {
            cacheEntry.lower = value;
            cacheEntry.upper = value;
            cacheEntry.bestMove = bestMove;
        } else if(value <= alpha) {
            cacheEntry.upper = value;
            cacheEntry.bestMove = MOVE_INVALID;
        } else if(value >= beta) {
            cacheEntry.lower = value;
            cacheEntry.bestMove = MOVE_INVALID;
        }
        cacheEntry.depth = depth;
        cache->put(state, cacheEntry);
    }

#ifdef DEBUG
    assert(bestMove != MOVE_INVALID);
#endif

    if(depth <= 33) { std::cout << state.print() << std::endl; }
    if(depth <= 33) { std::cout << "Value = " << printValue(value) << std::endl; }


    return value;
}

void Connect4::principleVariation(const GameState& board, Move buffer[DEPTH_MAX]) {

    for(unsigned i=0; i<DEPTH_MAX; i++) {
        buffer[i] = MOVE_INVALID;
    }

    GameState state = board;
    Move bestMove;
    Entry cacheEntry;
    for(Depth depth=board.getDepth(); depth<DEPTH_MAX; depth++) {
        std::cout << "Depth=" << (int)depth << std::endl;
        GameState children[WIDTH];
        state.children(children, this->width, this->height);
        std::cout << state.print() << std::endl;

        Value value = state.evaluate();
        if(value != VALUE_UNKNOWN) {
            buffer[depth] = MOVE_INVALID;
        } else if(depth >= this->width * this->height) {
            buffer[depth] = MOVE_INVALID;
        } else {

            if(cache->get(state, cacheEntry) && cacheEntry.bestMove != MOVE_INVALID) {
                std::cout << "Cache hit" << std::endl;
                std::cout << "Best move=" << (int)cacheEntry.bestMove << std::endl;
                buffer[depth] = cacheEntry.bestMove;
                state = children[cacheEntry.bestMove];
            } else {
                std::cout << "Cache miss" << std::endl;
                alphaBeta(state, VALUE_MIN, VALUE_MAX, bestMove);
                std::cout << "Best move=" << (int)bestMove << std::endl;
                buffer[depth] = bestMove;
                state = children[bestMove];
            }
        }
    }
}
