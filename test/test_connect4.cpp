#include "catch.hpp"
#include "connect4.hpp"

Value minimax(const GameState& board, int maxDepth) {
    Value value = board.evaluate();
    if(value != VALUE_UNKNOWN) {
        return value;
    } else if(board.getDepth() >= maxDepth) {
        return board.heuristic();
    } else {
        GameState buffer[WIDTH];
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

TEST_CASE("Connect4::alphaBeta", "[slow][hide]") {
    for(int i=0; i<10000; i++) {
        Connect4 game(16);

#if WIDTH == 6 && HEIGHT == 5
        int startDepth = 18;
        int maxDepth = 30;
#elif WIDTH == 7 && HEIGHT == 6
        int startDepth = 32;
        int maxDepth = 42;
#endif

        GameState board = GameState::random(startDepth);

        Value minimaxValue = minimax(board, maxDepth);
        Value alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, maxDepth);

        REQUIRE(minimaxValue == alphaBetaValue);
    }
}

