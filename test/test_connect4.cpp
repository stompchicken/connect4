#include "catch.hpp"
#include "connect4.hpp"

Value minimax(const GameState& board, unsigned* variation) {
    Depth depth = board.getDepth();
    Value value = board.evaluate();

#ifdef DEBUG
    assert(depth < DEPTH_MAX);
#endif

    if(value != VALUE_UNKNOWN) {
        return value;
    } else {
        GameState buffer[WIDTH];
        unsigned pv[DEPTH_MAX];
        board.children(buffer);

        value = VALUE_UNKNOWN;
        for(int i=0; i<WIDTH; i++) {
            if(buffer[i].isValid()) {

                Value val = minimax(buffer[i], pv);

                if(value == VALUE_UNKNOWN ||
                   (board.getPlayer() == PLAYER_MAX && val > value) ||
                   (board.getPlayer() == PLAYER_MIN && val < value)) {

                    value = val;
                    variation[depth] = i;
                    for(int i=depth+1; i<DEPTH_MAX; i++) {
                        variation[i] = pv[i];
                    }
                }
            }
        }

        return value;
    }
}



TEST_CASE("Connect4::alphaBeta", "[fast]") {

    Connect4 game(16);

    GameState board = GameState::parse(
        "X|.|.|X|.|.|\n"
        "O|.|.|O|.|.|\n"
        "X|O|.|X|X|.|\n"
        "O|X|X|O|O|.|\n"
        "X|O|O|X|O|X|\n");
    // Winning line: [1, 1, 2]
    REQUIRE(game.alphaBeta(board, VALUE_MIN, VALUE_MAX) == VALUE_MIN);
}

TEST_CASE("Connect4::minimax", "[slow][hide]") {
    for(int i=0; i<1000; i++) {
        Connect4 game(16);

#if WIDTH == 6 && HEIGHT == 5
        int startDepth = 16;
#elif WIDTH == 7 && HEIGHT == 6
        int startDepth = 32;
        int maxDepth = 42;
#endif

        GameState board = GameState::random(startDepth);

        unsigned mmpv[DEPTH_MAX];
        Value minimaxValue = minimax(board, mmpv);
        Value alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX);

        INFO(board.print());
        REQUIRE(minimaxValue == alphaBetaValue);

    }
}

