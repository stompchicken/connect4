#include "catch.hpp"
#include "moveOrder.hpp"

TEST_CASE("Moves::staticOrder") {

    MoveOrder moveOrder;
    Move moves[WIDTH];
    moveOrder.getStaticOrder(moves);
    REQUIRE(moves[0] == 3);
    REQUIRE(moves[1] == 2);
    REQUIRE(moves[2] == 4);
    REQUIRE(moves[3] == 1);
    REQUIRE(moves[4] == 5);
    REQUIRE(moves[5] == 0);
    REQUIRE(moves[6] == 6);

    Move lr[WIDTH] = {0, 1, 2, 3, 4, 5, 6};
    moveOrder.setStaticOrder(lr);
    moveOrder.getStaticOrder(moves);
    REQUIRE(moves[0] == 0);
    REQUIRE(moves[1] == 1);
    REQUIRE(moves[2] == 2);
    REQUIRE(moves[3] == 3);
    REQUIRE(moves[4] == 4);
    REQUIRE(moves[5] == 5);
    REQUIRE(moves[6] == 6);
}


TEST_CASE("Moves::reorder") {

    MoveOrder moveOrder;
    Move moves[WIDTH];
    Depth depth = 2;

    moveOrder.reorder(1, MOVE_INVALID, moves);

    // Static move ordering
    // 0  1  2  3  4  5, 6
    //{2, 4, 6, 7, 5, 3, 1}
    REQUIRE(moves[0] == 3);
    REQUIRE(moves[1] == 2);
    REQUIRE(moves[2] == 4);
    REQUIRE(moves[3] == 1);
    REQUIRE(moves[4] == 5);
    REQUIRE(moves[5] == 0);

    // Best cutoff move is pushed to the top
    moveOrder.recordCutoffMove(depth, 4);
    moveOrder.reorder(depth, MOVE_INVALID, moves);
    REQUIRE(moves[0] == 4);
    REQUIRE(moves[1] == 3);
    REQUIRE(moves[2] == 2);
    REQUIRE(moves[3] == 1);
    REQUIRE(moves[4] == 5);
    REQUIRE(moves[5] == 0);

    // Supplied best move is pushed to the top
    moveOrder.reorder(depth, 1, moves);
    REQUIRE(moves[0] == 1);
    REQUIRE(moves[1] == 4);
    REQUIRE(moves[2] == 3);
    REQUIRE(moves[3] == 2);
    REQUIRE(moves[4] == 5);
    REQUIRE(moves[5] == 0);

}

