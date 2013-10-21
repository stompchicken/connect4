#include "perfect.hpp"
#include "gtest/gtest.h"

using namespace testing;

class PerfectTest : public Test {};

Value minimax(const Connect4& board, int maxDepth) {
    Value value = board.evaluate();
    if(value != VALUE_UNKNOWN) {
        return value;
    } else if(board.getDepth() >= maxDepth) {
        return board.heuristic();
    } else {
        Connect4 buffer[WIDTH];
        board.children(buffer);

        value = (board.getPlayer() == PLAYER_MAX) ? VALUE_MIN : VALUE_MAX;
        for(int i=0; i<WIDTH; i++) {
            if(buffer[i].isValid()) {
                if(board.getPlayer() == PLAYER_MAX) {
                    value = std::max(value, minimax(buffer[i], maxDepth));
                } else {
                    value = std::min(value, minimax(buffer[i], maxDepth));
                }
            }
        }

        return value;
    }
}

TEST_F(PerfectTest, AlphaBeta) {
    Game game(16);
    for(int i=0; i<10; i++) {
        int startDepth = 16;
        int maxDepth = startDepth + 4;

        Connect4 board = Connect4::random(startDepth);

        Value minimaxValue = minimax(board, maxDepth);
        Value alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, maxDepth);

        ASSERT_EQ(minimaxValue, alphaBetaValue);
        game.reset();
    }
}
