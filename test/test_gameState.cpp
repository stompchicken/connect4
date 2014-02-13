#include "catch.hpp"
#include "gameState.hpp"

void assertEval(std::string text, int value) {
    GameState state;
    state.parse(text, PLAYER_MAX, 0);
    REQUIRE(value == state.evaluate());
    state.flip();
    REQUIRE(flipValue(value) == state.evaluate());
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
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|",
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

#elif WIDTH == 6 && HEIGHT == 5

TEST_CASE("GameState::evaluate", "[fast]") {
    assertEval(
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|\n"
        ".|.|.|.|.|.|",
        VALUE_UNKNOWN);
}
#endif

TEST_CASE("GameState::parse", "[fast]") {
  for(int i=0; i<1000; i++) {
      GameState board = GameState::random();
      board.assertInvariants();
      GameState parsed = GameState();
      parsed.parse(board.print(), board.getPlayer(), board.getDepth());
      parsed.assertInvariants();
      REQUIRE(board == parsed);
  }
}

/*
class BitboardTest : public Test { };

TEST_F(BitboardTest, Line4) {
    uint64 b = Bitboard::parse(
        ".|.|.|.|.|.|X|\n"
        ".|.|X|.|X|X|X|\n"
        "X|X|X|.|.|X|X|\n"
        ".|.|X|.|X|.|.|\n"
        ".|X|X|X|X|.|.|\n"
        ".|.|.|.|X|.|.|");

    uint64 lines = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|X|.|.|.|X|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|X|.|.|\n"
        ".|.|.|.|X|.|.|");

    ASSERT_EQ(lines, Bitboard::line4(b));
}

TEST_F(BitboardTest, Line3) {
    uint64 b = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|X|.|X|.|\n"
        ".|X|.|.|.|X|.|\n"
        ".|X|.|X|.|.|.|\n"
        ".|X|.|X|.|X|.|\n"
        ".|.|.|.|.|.|.|");

    uint64 lines = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        ".|X|.|.|.|.|.|\n"
        ".|.|.|X|.|.|.|\n"
        ".|.|.|.|.|X|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|X|.|.|.|.|.|");

    ASSERT_EQ(lines, Bitboard::line3(b)) << Bitboard::print(Bitboard::line3(b));

    b = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|X|X|X|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|X|X|.|X|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|X|.|X|X|.|.|");

    lines = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        ".|X|.|.|.|X|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|X|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|X|.|.|.|.|");

    ASSERT_EQ(lines, Bitboard::line3(b)) << Bitboard::print(Bitboard::line3(b));

    b = Bitboard::parse(
        ".|.|.|X|.|.|.|\n"
        ".|.|X|.|.|.|.|\n"
        "X|.|.|X|.|X|.|\n"
        ".|X|.|.|X|.|X|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|X|.|.|.|");

    lines = Bitboard::parse(
        ".|X|.|.|.|.|.|\n"
        ".|.|.|.|X|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|X|.|.|X|.|\n"
        ".|.|.|.|.|.|.|");

    ASSERT_EQ(lines, Bitboard::line3(b)) << Bitboard::print(Bitboard::line3(b));

    b = Bitboard::parse(
        ".|.|.|X|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|X|.|.|X|.|X|\n"
        "X|.|.|X|.|X|.|\n"
        ".|.|X|.|.|.|.|\n"
        ".|.|.|X|.|.|.|");

    lines = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|X|.|.|X|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|X|.|.|\n"
        ".|X|.|.|.|.|.|");

    ASSERT_EQ(lines, Bitboard::line3(b)) << Bitboard::print(Bitboard::line3(b));
}

class DISABLEDConnect4Test : public Test {
  public:
    virtual void SetUp() {
        srand(8088);
    }
};

TEST_F(Connect4Test, Parse) {
  for(int i=0; i<1000; i++) {
      Connect4 board = Connect4::random();
      board.assertInvariants();
      Connect4 parsed = Connect4();
      parsed.parse(board.print(), board.getPlayer(), board.getDepth());
      parsed.assertInvariants();
      ASSERT_EQ(board, parsed) << i << std::endl << board.print() << " != " << parsed.print() << std::endl;;
  }
}

void assertEval(std::string text, int value) {
    Connect4 board;
    board.parse(text, PLAYER_MAX, 0);
    ASSERT_EQ(value, board.evaluate()) << text << std::endl << board.print();
//    board.flipBoard();
//    ASSERT_EQ(flipValue(value), board.evaluate()) << board.print();
}

TEST_F(Connect4Test, Evaluate) {

}

TEST_F(Connect4Test, Children) {
    Connect4 board;
    board.parse(
      ".|.|O|.|.|.|X|\n"
      ".|.|O|X|.|.|X|\n"
      ".|.|O|X|O|.|X|\n"
      ".|.|O|X|O|.|X|\n"
      ".|X|O|X|O|.|X|\n"
      "O|X|O|X|O|.|X|",
      PLAYER_MAX, 1);

    Connect4 buffer[WIDTH];
    board.children(buffer);

    std::string childBoard[WIDTH] = {
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
        if(childBoard[i].size() > 0) {
            Connect4 child;
            child.parse(childBoard[i], PLAYER_MIN, 2);
            child.assertInvariants();
            ASSERT_EQ(child, buffer[i]);
        } else {
            ASSERT_FALSE(buffer[i].isValid()) << buffer[i] << buffer[i].isValid();
        }
    }
}

TEST_F(Connect4Test, Flip) {

    Connect4 board;
    board.parse(
      ".|.|.|.|.|.|.|\n"
      ".|.|.|.|.|.|.|\n"
      ".|.|.|.|.|.|.|\n"
      ".|.|.|O|.|.|.|\n"
      ".|X|.|X|.|.|O|\n"
      "X|X|.|O|.|.|O|",
      PLAYER_MAX, 1);

    Connect4 flip;
    flip.parse(
      ".|.|.|.|.|.|.|\n"
      ".|.|.|.|.|.|.|\n"
      ".|.|.|.|.|.|.|\n"
      ".|.|.|O|.|.|.|\n"
      "O|.|.|X|.|X|.|\n"
      "O|.|.|O|.|X|X|",
      PLAYER_MIN, 1);


    ASSERT_EQ(flip, board.flip()) << board.flip().print();
}

*/
