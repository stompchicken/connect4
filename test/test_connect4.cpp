#include "connect4.hpp"
#include "gtest/gtest.h"

using namespace testing;

class Connect4Test : public Test {
  public:
    virtual void SetUp() {
        srand(12);
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
    board.flipBoard();
    ASSERT_EQ(flipValue(value), board.evaluate()) << board.print();
}

TEST_F(Connect4Test, Evaluate) {

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

TEST_F(Connect4Test, Heuristic) {

    Connect4 board;
    board.parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|X|X|X|.|.|", PLAYER_MAX, 2);

    ASSERT_EQ(128 + 32, board.heuristic()) << board.print();

}

