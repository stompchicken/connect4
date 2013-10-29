#include "game.hpp"

#include <iostream>
#include <algorithm>
#include <cassert>

int flipPlayer(int player) {
    return player == PLAYER_MAX ? PLAYER_MIN : PLAYER_MAX;
}

Value flipValue(Value value) {
    if(value == VALUE_UNKNOWN) return VALUE_UNKNOWN;
    return 256 - value;
}

Value Game::alphaBeta(const Connect4& board, Value alpha, Value beta, int maxDepth) {
//    std::cout << board.getDepth() << " " << board.getPlayer() << std::endl;
//    std::cout << board.print() << std::endl << std::endl;

    stats->nodesExplored++;
    int depth = board.getDepth();
    int player = board.getPlayer();

    // Look up state in cache
    CacheValue cacheValue;
    if(cache->get(board, cacheValue)) {
        stats->cacheHits++;
        if(cacheValue.lower >= beta) {
            return cacheValue.lower;
        } else if(alpha >= cacheValue.upper) {
            return cacheValue.upper;
        }
        alpha = std::max(alpha, cacheValue.lower);
        beta = std::min(beta, cacheValue.upper);
    } else {
        stats->cacheMisses++;
        cacheValue.lower = VALUE_MIN;
        cacheValue.upper = VALUE_MAX;
        cacheValue.move = MOVE_INVALID;
        cacheValue.depth = depth;
    }

    Value value = board.evaluate();
    if(value != VALUE_UNKNOWN) {
        stats->terminalNodes += 1;
        return value;
    } else if(depth >= maxDepth) {
        return board.heuristic();
    } else {
        Connect4* children = bufferStart;
        board.children(children);
        bufferStart += WIDTH;

        Value a = alpha;
        Value b = beta;
        int bestMove = 0;
        value = (player == PLAYER_MAX) ? VALUE_MIN : VALUE_MAX;

        NodeOrdering ordering(children, player);
//        std::cout << ordering << std::endl;
        for(unsigned i=0; i<WIDTH; i++) {
            int move = ordering.move(i);
            Connect4& child = children[move];

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
                stats->cutoffs += 1;
                break;
            }
        }

        if(value <= alpha) {
            cacheValue.upper = value;
            cacheValue.move = bestMove;
        } else if(value > alpha && value < beta) {
            cacheValue.lower = value;
            cacheValue.upper = value;
            cacheValue.move = bestMove;
        } else if(value >= beta) {
            cacheValue.lower = value;
            cacheValue.move = bestMove;
        }
        cache->put(board, cacheValue);


        bufferStart -= WIDTH;

        return value;
    }
}

std::ostream& operator<<(std::ostream& os, const Stats& stats) {
    os << stats.nodesExplored << "\t nodes explored" << std::endl;
    os << stats.cutoffs << "\t nodes cut off" << std::endl;
    os << stats.terminalNodes << "\t terminal nodes" << std::endl;
    os << stats.cacheHits << "\t cache hits" << std::endl;
    os << stats.cacheMisses << "\t cache misses" << std::endl;
    os << static_cast<float>(stats.cacheHits)/(stats.cacheHits+stats.cacheMisses) << "\t cache hit rate" << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, NodeOrdering const& o) {
    os << "NodeOrdering: {";
    for(unsigned i=0; i<WIDTH; i++) {
        os << "(" << o.ordering[i].index << ", " << o.ordering[i].score << ") ";
    }
    return os;
}
