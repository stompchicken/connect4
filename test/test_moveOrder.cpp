#include "catch.hpp"
#include "moveOrder.hpp"

#if WIDTH == 7

#else

TEST_CASE("Moves::reorder", "[fast]") {

    MoveOrder moveOrder;

    GameState parent = GameState::parse(
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|X|X|.|.|\n");

    unsigned moves[WIDTH];
    GameState children[WIDTH];

    moveOrder.reorder(1, MOVE_INVALID, moves);
    parent.children(children);

    // 0  1  2  3  4  5
    //{2, 4, 6, 7, 5, 3}

    REQUIRE(moves[0] == 3);
    REQUIRE(moves[1] == 2);
    REQUIRE(moves[2] == 4);
    REQUIRE(moves[3] == 1);
    REQUIRE(moves[4] == 5);
    REQUIRE(moves[5] == 0);

    moveOrder.cutoff(2, 4);

    moveOrder.reorder(2, MOVE_INVALID, moves);
    REQUIRE(moves[0] == 4);
    REQUIRE(moves[1] == 3);
    REQUIRE(moves[2] == 2);
    REQUIRE(moves[3] == 1);
    REQUIRE(moves[4] == 5);
    REQUIRE(moves[5] == 0);

    moveOrder.reorder(2, 1, moves);
    REQUIRE(moves[0] == 1);
    REQUIRE(moves[1] == 4);
    REQUIRE(moves[2] == 3);
    REQUIRE(moves[3] == 2);
    REQUIRE(moves[4] == 5);
    REQUIRE(moves[5] == 0);

}

#endif
