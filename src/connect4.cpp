#include "connect4.hpp"

#define MAX_CACHE_DEPTH 26
#define MAX_LR_DEPTH 10

std::ostream& operator<<(std::ostream &output, const Stats &stats) {
    output << stats.nodesExplored << " nodes explored" << std::endl;
    output << stats.cutoffs << " cutoffs" << std::endl;

    float hitRate = float(stats.cacheHits)/float(stats.cacheHits+stats.cacheMisses);
    output << hitRate << " cache hitrate" << std::endl;

    return output;
}


void Connect4::orderChildren(GameState*, Player, Move* moves) {
/*
    for(unsigned i=0; i<WIDTH; i++) {
        moves[i].move = i;
        moves[i].value = abs(i - WIDTH/2);
    }

    if(player == PLAYER_MAX) {
        std::sort(moves, moves+WIDTH, orderMax);
    } else if (player == PLAYER_MIN) {
        std::sort(moves, moves+WIDTH, orderMin);
    }
*/

    moves[0].move = 2;
    moves[1].move = 3;
    moves[2].move = 1;
    moves[3].move = 4;
    moves[4].move = 0;
    moves[5].move = 5;
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

    if(depth < MAX_CACHE_DEPTH && cache->get(state, cacheEntry)) {
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
        unsigned bestMove = 0;
        Move moves[WIDTH];
        orderChildren(children, player, moves);

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
        if(depth < MAX_CACHE_DEPTH) {
            if(value <= alpha) {
                cacheEntry.upper = value;
            } else if (value > alpha && value < beta) {
                cacheEntry.lower = value;
                cacheEntry.upper = value;
                cacheEntry.bestMove = bestMove;
            } else if(value >= beta) {
                cacheEntry.lower = value;
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
