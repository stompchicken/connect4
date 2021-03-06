#include "connect4.hpp"

#include <math.h>
#include <sstream>

Value Connect4::solve(const GameState& state) {
    this->reset();
    moveOrder.reset();
    Move bestMove = MOVE_INVALID;
    Value value = alphaBeta(state, VALUE_MIN, VALUE_MAX, bestMove);
    return value;
}

Value Connect4::minimax(const GameState& board) {
    Depth depth = board.getDepth();
    Value value = board.evaluate();

#ifdef DEBUG
    assert(board.isValid());
    assert(depth <= DEPTH_MAX);
#endif

    if(value != VALUE_UNKNOWN) {
        return value;
    } else if (depth == config.width*config.height) {
        return VALUE_DRAW;
    } else {
        GameState buffer[WIDTH];
        board.children(buffer, config.width, config.height);

        value = VALUE_UNKNOWN;
        for(int i=0; i<WIDTH; i++) {
            if(buffer[i].isValid()) {
                Value val = minimax(buffer[i]);
                if(value == VALUE_UNKNOWN || 
                   (board.getPlayer() == PLAYER_MAX && val > value) ||
                   (board.getPlayer() == PLAYER_MIN && val < value)) {
                    value = val;
                }
            }
        }

        return value;
    }
}

Value Connect4::alphaBeta(const GameState& state, const Value alpha, const Value beta, Move& bestMove) {
    stats->nodesExplored++;

    const Depth depth = state.getDepth();
    const Player player = state.getPlayer();

#ifdef DEBUG
    assert(state.isValid());
    assert(depth >= 0 && depth <= DEPTH_MAX);
    assert(alpha < beta);
    bestMove = MOVE_INVALID;
#endif

    if(config.cachePrefetch) {
        cache->prefetch(state);
    }

    Value value = state.evaluate();
    if(value != VALUE_UNKNOWN) {
        return value;
    } else if(depth == config.height * config.width) {
        return VALUE_DRAW;
    }

    Value a = alpha;
    Value b = beta;

    // Look up state in cache
    Entry cacheEntry;
    if(config.useCache && depth <= config.cacheMaxDepth &&
       cache->get(state, cacheEntry)) {
            stats->cacheHits++;
            if(cacheEntry.lower == cacheEntry.upper) {
                bestMove = cacheEntry.bestMove;
                return cacheEntry.lower;
            } else if(cacheEntry.lower >= beta) {
                bestMove = MOVE_INVALID;
                return beta;
            } else if(cacheEntry.upper <= alpha) {
                bestMove = MOVE_INVALID;
                return alpha;
            }

            a = std::max(a, cacheEntry.lower);
            b = std::min(b, cacheEntry.upper);
    } else {
        stats->cacheMisses++;
        cacheEntry.lower = VALUE_MIN;
        cacheEntry.upper = VALUE_MAX;
    }

    // Generate children
    Move moves[WIDTH];
    GameState* children;
    PoolEntry<GameState> childArray = statePool.get(WIDTH);
    children = childArray.data;
    state.children(children, config.width, config.height);

    // Check for obvious winners
    if(config.preFilter) {
        for(Move move=0; move<WIDTH; move++) {
            const GameState& child = children[move];
            if(!child.isValid()) continue;
            value = child.evaluate();
            if(player == PLAYER_MAX && value == VALUE_MAX) {
                bestMove = move;
                return VALUE_MAX;
            }
            if(player == PLAYER_MIN && value == VALUE_MIN) {
                bestMove = move;
                return VALUE_MIN;
            }
        }
    }

    // Move ordering step
    if(config.reorderMoves) {
        moveOrder.reorder(depth+1, MOVE_INVALID, moves);
    } else {
        moveOrder.getStaticOrder(moves);
    }

    for(unsigned i=0; i<WIDTH; i++) {
        const Move move = moves[i];
        const GameState& child = children[move];

        if(!child.isValid()) continue;

        Move childBestMove;
        Value childVal = alphaBeta(child, a, b, childBestMove);
        if(player == PLAYER_MAX) {
            if(value == VALUE_UNKNOWN || childVal > value) {
                value = childVal;
                bestMove = move;
                moveOrder.recordCutoffMove(depth+1, bestMove);
                a = std::max(a, value);
            }
        } else {
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
    if(config.useCache && depth <= config.cacheMaxDepth) {

        // We need beta == VALUE_MAX because we may have cutoff on a
        // draw because of our bounds, which does not mean a draw is
        // the best possible score
        if(player == PLAYER_MAX && beta == VALUE_MAX) {
            cacheEntry.upper = a;
        } else if(player == PLAYER_MIN && alpha == VALUE_MIN){
            cacheEntry.lower = b;
        }

        if(cacheEntry.lower == cacheEntry.upper) {
            cacheEntry.bestMove = bestMove;
        } else {
            cacheEntry.bestMove = MOVE_INVALID;
        }

        cacheEntry.depth = depth;
        cache->put(state, cacheEntry);
    }

#ifdef DEBUG
    assert(bestMove != MOVE_INVALID);
#endif
    
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
//        std::cout << "Depth=" << (int)depth << std::endl;
        GameState children[WIDTH];
        state.children(children, config.width, config.height);


        Value value = state.evaluate();
        if(value != VALUE_UNKNOWN) {
            buffer[depth] = MOVE_INVALID;
        } else if(depth >= config.width * config.height) {
            buffer[depth] = MOVE_INVALID;
        } else {

            if(cache->get(state, cacheEntry) && cacheEntry.bestMove != MOVE_INVALID) {
//                std::cout << "Cache hit" << std::endl;
//                std::cout << "Best move=" << (int)cacheEntry.bestMove << std::endl;
                buffer[depth] = cacheEntry.bestMove;
                state = children[cacheEntry.bestMove];
            } else {
//                std::cout << "Cache miss" << std::endl;
                alphaBeta(state, VALUE_MIN, VALUE_MAX, bestMove);
//                std::cout << "Best move=" << (int)bestMove << std::endl;
                buffer[depth] = bestMove;
                state = children[bestMove];
            }
        }
    }
}


std::ostream& operator<<(std::ostream &output, const Config &config) {
    output << "{boardSize: " << config.width << "x" << config.height << ", ";
    output << "caching: " << (config.useCache ? "enabled" : "disabled") << ", ";
    if(config.useCache) {
        output << "cacheSize: " << config.cacheSize << ", ";
        output << "cacheProbe: " << config.cacheProbe << ", ";
        output << "cacheMaxDepth: " << (int)config.cacheMaxDepth << ", ";
    }

    output << "reordering: " << (config.reorderMoves ? "enabled" : "disabled") << ", ";
    output << "prefilter: " << (config.preFilter ? "enabled" : "disabled") << "}";

    return output;
}
