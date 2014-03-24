#include "connect4.hpp"
#include <algorithm>

#include <math.h>

#define MAX_CACHE_DEPTH WIDTH*HEIGHT

std::ostream& operator<<(std::ostream &output, const Stats &stats) {
    output << static_cast<float>(stats.nodesExplored) << " nodes explored" << std::endl;
    output << static_cast<float>(stats.cutoffs) << " cutoffs" << std::endl;

    float hitRate = float(stats.cacheHits)/float(stats.cacheHits+stats.cacheMisses);
    output << hitRate << " cache hitrate" << std::endl;

    return output;
}

#define SWAP_MAX(x, y) if(moves[x].value < moves[y].value) { temp = moves[y]; moves[y] = moves[x]; moves[x] = temp;  }
#define SWAP_MIN(x, y) if(moves[x].value > moves[y].value) { temp = moves[y]; moves[y] = moves[x]; moves[x] = temp;  }

uint8 moveOrderMax[WIDTH] = {2, 4, 6, 7, 5, 3};
uint8 moveOrderMin[WIDTH] = {6, 4, 2, 1, 3, 5};

void MoveOrdering::orderMoves(GameState* children, unsigned bestMove, Player player, Move* moves) {
    for(unsigned i=0; i<WIDTH; i++) {
        moves[i].move = i;

        if(player == PLAYER_MAX) {
            moves[i].value = moveOrderMax[i];
//            if(i == bestMove) moves[i].value = 100;
        } else {
            moves[i].value = moveOrderMin[i];
//            if(i == bestMove) moves[i].value = 0;
        }
    }

    Move temp;
    if(player == PLAYER_MAX) {

#if WIDTH == 7
        SWAP_MAX(1, 2); SWAP_MAX(0, 2); SWAP_MAX(0, 1); SWAP_MAX(3, 4);
        SWAP_MAX(5, 6); SWAP_MAX(3, 5); SWAP_MAX(4, 6); SWAP_MAX(4, 5);
        SWAP_MAX(0, 4); SWAP_MAX(0, 3); SWAP_MAX(1, 5); SWAP_MAX(2, 6);
        SWAP_MAX(2, 5); SWAP_MAX(1, 3); SWAP_MAX(2, 4); SWAP_MAX(2, 3);
#else
        SWAP_MAX(1, 2); SWAP_MAX(0, 2); SWAP_MAX(0, 1); SWAP_MAX(4, 5);
        SWAP_MAX(3, 5); SWAP_MAX(3, 4); SWAP_MAX(0, 3); SWAP_MAX(1, 4);
        SWAP_MAX(2, 5); SWAP_MAX(2, 4); SWAP_MAX(1, 3); SWAP_MAX(2, 3);
#endif

    } else if (player == PLAYER_MIN) {
#if WIDTH == 7
        SWAP_MIN(1, 2); SWAP_MIN(0, 2); SWAP_MIN(0, 1); SWAP_MIN(3, 4);
        SWAP_MIN(5, 6); SWAP_MIN(3, 5); SWAP_MIN(4, 6); SWAP_MIN(4, 5);
        SWAP_MIN(0, 4); SWAP_MIN(0, 3); SWAP_MIN(1, 5); SWAP_MIN(2, 6);
        SWAP_MIN(2, 5); SWAP_MIN(1, 3); SWAP_MIN(2, 4); SWAP_MIN(2, 3);
#else
        SWAP_MIN(1, 2); SWAP_MIN(0, 2); SWAP_MIN(0, 1); SWAP_MIN(4, 5);
        SWAP_MIN(3, 5); SWAP_MIN(3, 4); SWAP_MIN(0, 3); SWAP_MIN(1, 4);
        SWAP_MIN(2, 5); SWAP_MIN(2, 4); SWAP_MIN(1, 3); SWAP_MIN(2, 3);
#endif
    }


}

Value Connect4::solve(const GameState& state) {
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

/*
            if(cacheEntry.bestMove != MOVE_INVALID) {
                bestMove = cacheEntry.bestMove;
            }
*/
        } else {
            stats->cacheMisses++;
            cacheEntry.lower = VALUE_MIN;
            cacheEntry.upper = VALUE_MAX;
        }

        GameState* children = bufferStart;
        state.children(children);
        bufferStart += WIDTH;

        Value a = alpha;
        Value b = beta;
        MoveOrdering::Move moves[WIDTH];
        MoveOrdering::orderMoves(children, bestMove, player, moves);

        for(unsigned i=0; i<WIDTH; i++) {
            const unsigned move = moves[i].move;
            const GameState& child = children[move];

            if(!child.isValid()) continue;

            Value childVal;
            if(player == PLAYER_MAX) {
                childVal = alphaBeta(child, a, beta);
                if(value == VALUE_UNKNOWN || childVal > value) {
                    value = childVal;
                    bestMove = move;
                    a = std::max(a, value);
                }
            } else {
                childVal = alphaBeta(child, alpha, b);
                if(value == VALUE_UNKNOWN || childVal < value) {
                    value = childVal;
                    bestMove = move;
                    b = std::min(b, value);
                }
            }

            if(a >= b) {
                stats->cutoffs++;
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
        bufferStart -= WIDTH;
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
