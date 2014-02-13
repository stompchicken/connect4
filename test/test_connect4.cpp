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

TEST_CASE("Connect4::alphaBeta", "[slow]") {
    for(int i=0; i<1000; i++) {
        Connect4 game(20);
        int startDepth = (rand() % 8) + 8;
        int maxDepth = startDepth + 8;

        GameState board = GameState::random(startDepth);

        Value minimaxValue = minimax(board, maxDepth);
        Value alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, maxDepth);

        REQUIRE(minimaxValue == alphaBetaValue);
    }
}
