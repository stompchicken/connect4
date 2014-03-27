#include "connect4.hpp"
#include <algorithm>

#include <math.h>

#define MAX_CACHE_DEPTH WIDTH*HEIGHT - 6

std::ostream& operator<<(std::ostream &output, const Stats &stats) {
    output << static_cast<float>(stats.nodesExplored) << " nodes explored" << std::endl;
    output << static_cast<float>(stats.cutoffs) << " cutoffs" << std::endl;

    float hitRate = float(stats.cacheHits)/float(stats.cacheHits+stats.cacheMisses);
    output << hitRate << " cache hitrate" << std::endl;

    return output;
}

std::ostream& operator<<(std::ostream &output, const MoveOrdering &moveOrdering) {
    output << "{ MoveOrdering " << std::endl;
    for(unsigned d=0; d<DEPTH_MAX; d++) {
        std::cout << d << " ";
        for(unsigned i=0; i<WIDTH; i++) {
            output << "(" << moveOrdering.moves[d*i].move << "," << moveOrdering.moves[d*i].value << ") ";
        }
        output << std::endl;
    }
    output << "}";
    return output;
}


#define SWAP(x, y) if(moveBuffer[x].value < moveBuffer[y].value) { temp = moveBuffer[y]; moveBuffer[y] = moveBuffer[x]; moveBuffer[x] = temp;  }

uint8 staticMoveOrder[WIDTH] = {2, 4, 6, 7, 5, 3};

void MoveOrdering::orderMoves(const GameState& parent, GameState*, unsigned) {
    Depth depth = parent.getDepth();
//    Player player = parent.getPlayer();

    Move* moveBuffer = getMoves(depth);

    for(unsigned i=0; i<WIDTH; i++) {
        moveBuffer[i].move = i;
        moveBuffer[i].value = staticMoveOrder[i];
        if(i == killerMove[depth]) {
            moveBuffer[i].value = 100;
        }
    }

    Move temp;
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

#ifdef DEBUG
    for(unsigned i=0; i<WIDTH-1; i++) {
//        std::cout << *this << std::endl;
        assert(moveBuffer[i].move < WIDTH);
        assert(moveBuffer[i].value >= moves[i+1].value);
    }
#endif
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

    if(state.key() == 6808838541) {
        std::cout << "Solving..." << std::endl;
        std::cout << state.print() << std::endl;
        std::cout << "alpha=" << printValue(alpha) << std::endl;
        std::cout << "beta=" << printValue(beta) << std::endl;
    }

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
        moveOrdering.orderMoves(state, children, bestMove);


        if(state.key() == 6808838541) {
            std::cout << moveOrdering << std::endl;
        }

        MoveOrdering::Move* moves = moveOrdering.getMoves(depth);

            if(state.key() == 6808838541) {
                
        for(unsigned i=0; i<WIDTH; i++) {
            std::cout << "(" << moves[i].move << " " << moves[i].value << ") ";
        }
        std::cout << std::endl;
            }

        for(unsigned i=0; i<WIDTH; i++) {
            const unsigned move = moves[i].move;
            const GameState& child = children[move];

            if(state.key() == 6808838541) {
                std::cout << "Move=" << move << std::endl;
            }

            if(!child.isValid()) continue;


            Value childVal;
            if(player == PLAYER_MAX) {
                childVal = alphaBeta(child, a, beta);
                if(value == VALUE_UNKNOWN || childVal > value) {
                    value = childVal;
                    bestMove = move;
                    moveOrdering.killerMove[depth] = move;
                    a = std::max(a, value);
                }
            } else {
                childVal = alphaBeta(child, alpha, b);
                if(value == VALUE_UNKNOWN || childVal < value) {
                    value = childVal;
                    bestMove = move;
                    moveOrdering.killerMove[depth] = move;
                    b = std::min(b, value);
                }
            }

            if(a >= b) {
                stats->cutoffs++;
                break;
            }
        }


        if(state.key() == 6808838541) {
            std::cout << "Solved..." << std::endl;
            std::cout << state.print() << std::endl;
            std::cout << printValue(value) << std::endl;
            std::cout << (int)bestMove << std::endl;
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
