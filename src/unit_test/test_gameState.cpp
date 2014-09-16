#include "catch.hpp"
#include "gameState.hpp"

void assertEval(std::string text, int value) {
    GameState state = GameState::parse(text);
    CHECK(value == state.evaluate());
}

TEST_CASE("GameState::evaluate") {
    assertEval(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|",
        VALUE_UNKNOWN);

    assertEval(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|X|\n"
        ".|.|.|.|.|.|X|\n"
        ".|.|.|.|.|.|X|\n"
        ".|.|.|.|.|.|X|",
        VALUE_MAX);

    assertEval(
        "X|.|.|.|.|.|.|\n"
        "X|.|.|.|.|.|.|\n"
        "X|.|.|.|.|.|.|\n"
        "X|.|.|.|.|.|.|\n"
        "O|.|.|.|.|.|.|\n"
        "O|.|.|.|.|.|.|",
        VALUE_MAX);


    assertEval(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        "X|X|X|X|.|.|.|",
        VALUE_MAX);

    assertEval(
        ".|.|.|X|X|X|X|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|",
        VALUE_MAX);

    assertEval(
        ".|.|.|X|.|.|.|\n"
        ".|.|X|.|.|.|.|\n"
        ".|X|.|.|.|.|.|\n"
        "X|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|",
        VALUE_MAX);

    assertEval(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|X|\n"
        ".|.|.|.|.|X|.|\n"
        ".|.|.|.|X|.|.|\n"
        ".|.|.|X|.|.|.|",
        VALUE_MAX);

    assertEval(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        "X|.|.|.|.|.|.|\n"
        ".|X|.|.|.|.|.|\n"
        ".|.|X|.|.|.|.|\n"
        ".|.|.|X|.|.|.|",
        VALUE_MAX);

    assertEval(
        ".|.|.|X|.|.|.|\n"
        ".|.|.|.|X|.|.|\n"
        ".|.|.|.|.|X|.|\n"
        ".|.|.|.|.|.|X|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|",
        VALUE_MAX);

    assertEval(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|X|X|\n"
        "X|X|.|.|.|.|.|\n"
        ".|.|.|.|.|X|X|\n"
        ".|.|.|.|.|.|.|",
        VALUE_UNKNOWN);

    assertEval(
        ".|.|.|X|.|X|.|\n"
        ".|.|.|X|.|X|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|X|.|.|\n"
        ".|.|.|.|X|.|.|",
        VALUE_UNKNOWN);

    assertEval(
        "O|X|O|X|O|X|O|\n"
        "O|X|O|X|O|X|O|\n"
        "X|O|X|O|X|O|X|\n"
        "X|O|X|O|X|O|X|\n"
        "O|X|O|X|O|X|O|\n"
        "O|X|O|X|O|X|O|",
        VALUE_DRAW);
}

TEST_CASE("GameState::children") {
    GameState state = GameState::parse(
        ".|.|X|.|.|.|O|\n"
        ".|.|O|O|.|.|X|\n"
        ".|.|X|X|O|.|O|\n"
        ".|.|O|O|X|.|X|\n"
        ".|X|X|X|O|.|O|\n"
        "X|O|O|O|X|.|X|");

    GameState buffer[WIDTH];
    state.children(buffer);

    std::string childStates[WIDTH] = {
        ".|.|X|.|.|.|O|\n"
        ".|.|O|O|.|.|X|\n"
        ".|.|X|X|O|.|O|\n"
        ".|.|O|O|X|.|X|\n"
        "X|X|X|X|O|.|O|\n"
        "X|O|O|O|X|.|X|",

        ".|.|X|.|.|.|O|\n"
        ".|.|O|O|.|.|X|\n"
        ".|.|X|X|O|.|O|\n"
        ".|X|O|O|X|.|X|\n"
        ".|X|X|X|O|.|O|\n"
        "X|O|O|O|X|.|X|",

        "",

        ".|.|X|X|.|.|O|\n"
        ".|.|O|O|.|.|X|\n"
        ".|.|X|X|O|.|O|\n"
        ".|.|O|O|X|.|X|\n"
        ".|X|X|X|O|.|O|\n"
        "X|O|O|O|X|.|X|",

        ".|.|X|.|.|.|O|\n"
        ".|.|O|O|X|.|X|\n"
        ".|.|X|X|O|.|O|\n"
        ".|.|O|O|X|.|X|\n"
        ".|X|X|X|O|.|O|\n"
        "X|O|O|O|X|.|X|",

        ".|.|X|.|.|.|O|\n"
        ".|.|O|O|.|.|X|\n"
        ".|.|X|X|O|.|O|\n"
        ".|.|O|O|X|.|X|\n"
        ".|X|X|X|O|.|O|\n"
        "X|O|O|O|X|X|X|",

        ""};

    for(int i=0; i<WIDTH; i++) {
        if(childStates[i].size() > 0) {
            GameState child = GameState::parse(childStates[i]);
            child.assertInvariants();
            REQUIRE(child == buffer[i]);
        } else {
            REQUIRE(!buffer[i].isValid());
        }
    }
}

TEST_CASE("GameState::children(reduced)") {

    GameState state = GameState::parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|O|.|.|.|\n"
        ".|.|.|X|.|.|.|\n"
        ".|.|.|O|.|.|.|\n"
        ".|X|X|X|O|.|.|\n"
        "X|O|O|O|X|.|.|");

    GameState buffer[WIDTH];
    state.children(buffer, 6, 5);

    std::string childStates[WIDTH] = {
        ".|.|.|.|.|.|.|\n"
        ".|.|.|O|.|.|.|\n"
        ".|.|.|X|.|.|.|\n"
        ".|.|.|O|.|.|.|\n"
        "X|X|X|X|O|.|.|\n"
        "X|O|O|O|X|.|.|",

        ".|.|.|.|.|.|.|\n"
        ".|.|.|O|.|.|.|\n"
        ".|.|.|X|.|.|.|\n"
        ".|X|.|O|.|.|.|\n"
        ".|X|X|X|O|.|.|\n"
        "X|O|O|O|X|.|.|",

        ".|.|.|.|.|.|.|\n"
        ".|.|.|O|.|.|.|\n"
        ".|.|.|X|.|.|.|\n"
        ".|.|X|O|.|.|.|\n"
        ".|X|X|X|O|.|.|\n"
        "X|O|O|O|X|.|.|",

        "",

        ".|.|.|.|.|.|.|\n"
        ".|.|.|O|.|.|.|\n"
        ".|.|.|X|.|.|.|\n"
        ".|.|.|O|X|.|.|\n"
        ".|X|X|X|O|.|.|\n"
        "X|O|O|O|X|.|.|",

        ".|.|.|.|.|.|.|\n"
        ".|.|.|O|.|.|.|\n"
        ".|.|.|X|.|.|.|\n"
        ".|.|.|O|.|.|.|\n"
        ".|X|X|X|O|.|.|\n"
        "X|O|O|O|X|X|.|",

        ""
    };

    for(int i=0; i<WIDTH; i++) {
        INFO(i);
        if(childStates[i].size() > 0) {
            GameState child = GameState::parse(childStates[i]);
            child.assertInvariants();
            REQUIRE(child == buffer[i]);
        } else {
            REQUIRE(!buffer[i].isValid());
        }
    }
}

TEST_CASE("GameState::parse") {

    std::string board =
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|X|.|X|.|.|\n"
        ".|X|O|O|O|.|.|";
    GameState state = GameState::parse(board);
    state.assertInvariants();
    REQUIRE(state.getDepth() == 6);
    REQUIRE(state.getPlayer() == PLAYER_MAX);
    REQUIRE(state.print() == board);

    board =
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|X|.|X|.|.|\n"
        ".|X|O|O|O|X|.|";

    state = GameState::parse(board);
    state.assertInvariants();
    REQUIRE(state.getDepth() == 7);
    REQUIRE(state.getPlayer() == PLAYER_MIN);
    REQUIRE(state.print() == board);
}

TEST_CASE("GameState::flipLeftRight") {
    GameState state = GameState::parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|X|.|.|\n"
        ".|.|X|.|O|.|.|\n"
        ".|X|O|O|X|O|.|");

    GameState flip = state.flipLeftRight();
    REQUIRE(flip.getPlayer() == state.getPlayer());
    REQUIRE(flip.getDepth() == state.getDepth());
    REQUIRE(flip.print() ==
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|X|.|.|.|.|\n"
            ".|.|O|.|X|.|.|\n"
            ".|O|X|O|O|X|.|");
}
