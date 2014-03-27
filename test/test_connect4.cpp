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

#if WIDTH == 7
TEST_CASE("Connect4::alphaBeta", "[fast]") {

    Connect4 game(1024);

    GameState board = GameState::parse(
        ".|.|.|.|.|.|.|\n"
        ".|X|.|.|X|.|.|\n"
        ".|O|.|.|O|.|.|\n"
        ".|X|O|.|X|X|.|\n"
        ".|O|X|X|O|O|.|\n"
        ".|X|O|O|X|O|X|\n");
    // Winning line: [2, 2, 3]
    REQUIRE(game.alphaBeta(board, VALUE_MIN, VALUE_MAX) == VALUE_MIN);
}
#else
TEST_CASE("Connect4::alphaBeta", "[fast]") {

    Connect4 game(1024);

    GameState board = GameState::parse(
        "X|.|.|X|.|.|\n"
        "O|.|.|O|.|.|\n"
        "X|O|.|X|X|.|\n"
        "O|X|X|O|O|.|\n"
        "X|O|O|X|O|X|\n");
    // Winning line: [1, 1, 2]
    REQUIRE(game.alphaBeta(board, VALUE_MIN, VALUE_MAX) == VALUE_MIN);
}
#endif

#if WIDTH == 7

#else
TEST_CASE("MoveOrdering::orderMoves", "[fast]") {

    MoveOrdering moveOrdering;
    GameState parent = GameState::parse(
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|X|.|.|.|\n");
    GameState children[WIDTH];
    parent.children(children);

    moveOrdering.killerMove[1] = MOVE_INVALID;
    moveOrdering.orderMoves(parent, children, MOVE_INVALID);

    // 0  1  2  3  4  5
    //{2, 4, 6, 7, 5, 3};

    REQUIRE(moveOrdering.moves[0].move == 3);
    REQUIRE(moveOrdering.moves[1].move == 2);
    REQUIRE(moveOrdering.moves[2].move == 4);
    REQUIRE(moveOrdering.moves[3].move == 1);
    REQUIRE(moveOrdering.moves[4].move == 5);
    REQUIRE(moveOrdering.moves[5].move == 0);

    moveOrdering.killerMove[1] = 5;
    moveOrdering.orderMoves(parent, children, MOVE_INVALID);

    REQUIRE(moveOrdering.moves[0].move == 5);
    REQUIRE(moveOrdering.moves[1].move == 3);
    REQUIRE(moveOrdering.moves[2].move == 2);
    REQUIRE(moveOrdering.moves[3].move == 4);
    REQUIRE(moveOrdering.moves[4].move == 1);
    REQUIRE(moveOrdering.moves[5].move == 0);


/*
    moveOrdering.orderMoves(parent, children, MOVE_INVALID);
    REQUIRE(moves[0].move == 3);
    REQUIRE(moves[1].move == 2);
    REQUIRE(moves[2].move == 4);
    REQUIRE(moves[3].move == 1);
    REQUIRE(moves[4].move == 5);
    REQUIRE(moves[5].move == 0);

    moveOrdering.orderMoves(children, MOVE_INVALID, PLAYER_MAX);
    MoveOrdering::orderMoves(children, MOVE_INVALID, PLAYER_MIN, moves);
    REQUIRE(moves[0].move == 3);
    REQUIRE(moves[1].move == 2);
    REQUIRE(moves[2].move == 4);
    REQUIRE(moves[3].move == 1);
    REQUIRE(moves[4].move == 5);
    REQUIRE(moves[5].move == 0);

    MoveOrdering::orderMoves(children, 4, PLAYER_MAX, moves);
    REQUIRE(moves[0].move == 4);
    REQUIRE(moves[1].move == 3);
    REQUIRE(moves[2].move == 2);
    REQUIRE(moves[3].move == 1);
    REQUIRE(moves[4].move == 5);
    REQUIRE(moves[5].move == 0);

    MoveOrdering::orderMoves(children, 1, PLAYER_MIN, moves);
    REQUIRE(moves[0].move == 1);
    REQUIRE(moves[1].move == 3);
    REQUIRE(moves[2].move == 2);
    REQUIRE(moves[3].move == 4);
    REQUIRE(moves[4].move == 5);
    REQUIRE(moves[5].move == 0);
*/
}
#endif

TEST_CASE("Connect4::minimax", "[slow][hide]") {
    for(int i=0; i<1000; i++) {
        Connect4 game(64*1024*1024);

#if WIDTH == 6
        int startDepth = 16;
#elif WIDTH == 7
        int startDepth = 32;
#endif

        GameState board = GameState::random(startDepth);

        unsigned mmpv[DEPTH_MAX];
        Value minimaxValue = minimax(board, mmpv);
        Value alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX);

        INFO(board.print());
        REQUIRE(minimaxValue == alphaBetaValue);
        std::cout << "." << std::flush;
        if((i+1) % 80 == 0) std::cout << std::endl;

    }
}

