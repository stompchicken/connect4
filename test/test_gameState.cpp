#include "catch.hpp"
#include "gameState.hpp"

void assertEval(std::string text, int value) {
    GameState state = GameState::parse(text);
    CHECK(value == state.evaluate());
}

#if WIDTH == 7 && HEIGHT == 6

TEST_CASE("GameState::evaluate", "[fast]") {
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

TEST_CASE("GameState::Children", "[fast]") {
    GameState state = GameState::parse(
        ".|.|O|.|.|.|X|\n"
        ".|.|O|X|.|.|X|\n"
        ".|.|O|X|O|.|X|\n"
        ".|.|O|X|O|.|X|\n"
        ".|X|O|X|O|.|X|\n"
        "O|X|O|X|O|.|X|",
        PLAYER_MAX, 1);

    GameState buffer[WIDTH];
    state.children(buffer);

    std::string childStates[WIDTH] = {
        ".|.|O|.|.|.|X|\n"
        ".|.|O|X|.|.|X|\n"
        ".|.|O|X|O|.|X|\n"
        ".|.|O|X|O|.|X|\n"
        "X|X|O|X|O|.|X|\n"
        "O|X|O|X|O|.|X|",

        ".|.|O|.|.|.|X|\n"
        ".|.|O|X|.|.|X|\n"
        ".|.|O|X|O|.|X|\n"
        ".|X|O|X|O|.|X|\n"
        ".|X|O|X|O|.|X|\n"
        "O|X|O|X|O|.|X|",

        "",

        ".|.|O|X|.|.|X|\n"
        ".|.|O|X|.|.|X|\n"
        ".|.|O|X|O|.|X|\n"
        ".|.|O|X|O|.|X|\n"
        ".|X|O|X|O|.|X|\n"
        "O|X|O|X|O|.|X|",

        ".|.|O|.|.|.|X|\n"
        ".|.|O|X|X|.|X|\n"
        ".|.|O|X|O|.|X|\n"
        ".|.|O|X|O|.|X|\n"
        ".|X|O|X|O|.|X|\n"
        "O|X|O|X|O|.|X|",

        ".|.|O|.|.|.|X|\n"
        ".|.|O|X|.|.|X|\n"
        ".|.|O|X|O|.|X|\n"
        ".|.|O|X|O|.|X|\n"
        ".|X|O|X|O|.|X|\n"
        "O|X|O|X|O|X|X|",

        ""};

    for(int i=0; i<WIDTH; i++) {
        if(childStates[i].size() > 0) {
            GameState child = GameState::parse(childStates[i], PLAYER_MIN, 2);
            child.assertInvariants();
            REQUIRE(child == buffer[i]);
        } else {
            REQUIRE(!buffer[i].isValid());
        }
    }
}

TEST_CASE("GameState::parse", "[fast]") {

    GameState state = GameState::parse(
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|X|.|X|.|\n"
        ".|X|O|O|O|.|");
    state.assertInvariants();
    REQUIRE(state.getDepth() == 6);
    REQUIRE(state.getPlayer() == PLAYER_MIN);


    state = GameState::parse(
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|X|.|X|.|\n"
        ".|X|O|O|O|X|");
    state.assertInvariants();
    REQUIRE(state.getDepth() == 7);
    REQUIRE(state.getPlayer() == PLAYER_MAX);
}

#elif WIDTH == 6 && HEIGHT == 5

TEST_CASE("GameState::evaluate", "[fast]") {
    assertEval(
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|",
        VALUE_UNKNOWN);

    assertEval(
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|X|\n"
        ".|.|.|.|.|X|\n"
        ".|.|.|.|.|X|\n"
        ".|.|.|.|.|X|",
        VALUE_MAX);

    assertEval(
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        "X|X|X|X|.|.|",
        VALUE_MAX);

    assertEval(
        ".|.|.|X|.|.|\n"
        ".|.|X|.|.|.|\n"
        ".|X|.|.|.|.|\n"
        "X|.|.|.|.|.|\n"
        ".|.|.|.|.|.|",
        VALUE_MAX);

    assertEval(
        ".|.|.|.|.|X|\n"
        ".|.|.|.|X|.|\n"
        ".|.|.|X|.|.|\n"
        ".|.|X|.|.|.|\n"
        ".|.|.|.|.|.|",
        VALUE_MAX);
}
#endif

