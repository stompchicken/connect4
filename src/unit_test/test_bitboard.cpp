#include "catch.hpp"
#include "bitboard.hpp"

TEST_CASE("Bitboard::parse") {
    Bitboard::Board board;
    board = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|", 'X');
    REQUIRE(board == 0);

    board = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        "O|.|.|.|.|.|.|\n"
        "O|.|.|.|.|.|.|\n"
        "O|.|.|.|.|.|.|\n"
        "O|.|.|.|.|.|.|", 'O');
    REQUIRE(board == 15);

    board = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|@|.|.|.|.|.|", '@');
    REQUIRE(board == 128);
}

TEST_CASE("Bitboard::print") {
    REQUIRE(Bitboard::print(0, 'X') ==
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|");

    REQUIRE(Bitboard::print(15, 'O') ==
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            "O|.|.|.|.|.|.|\n"
            "O|.|.|.|.|.|.|\n"
            "O|.|.|.|.|.|.|\n"
            "O|.|.|.|.|.|.|");

    REQUIRE(Bitboard::print(128, '@') ==
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|@|.|.|.|.|.|");

}

TEST_CASE("Bitboard::popcount") {
    REQUIRE(Bitboard::popcount(0) == 0);
    REQUIRE(Bitboard::popcount(15) == 4);
    REQUIRE(Bitboard::popcount(128) == 1);
}

TEST_CASE("Bitboard::flipLeftRight") {
    Bitboard::Board board;
    board = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|X|.|\n"
        ".|.|.|.|.|X|.|\n"
        ".|.|.|X|.|X|.|\n"
        ".|.|X|X|.|X|.|\n"
        ".|X|X|X|.|X|.|", 'X');
    board = Bitboard::flip(board);
    REQUIRE(Bitboard::print(board, 'X') ==
            ".|.|.|.|.|.|.|\n"
            ".|X|.|.|.|.|.|\n"
            ".|X|.|.|.|.|.|\n"
            ".|X|.|X|.|.|.|\n"
            ".|X|.|X|X|.|.|\n"
            ".|X|.|X|X|X|.|");
}

TEST_CASE("Bitboard::line4") {
    Bitboard::Board board;
    board = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|X|.|.|.|.|.|\n"
        ".|X|.|.|.|.|.|\n"
        ".|X|.|X|X|X|X|\n"
        ".|X|.|.|.|.|.|", 'X');
    REQUIRE(Bitboard::print(Bitboard::line4(board), 'X') ==
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|X|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|X|\n"
            ".|.|.|.|.|.|.|");

    board = Bitboard::parse(
        ".|.|.|.|.|.|.|\n"
        "X|.|.|.|.|.|.|\n"
        ".|X|.|.|.|.|X|\n"
        ".|.|X|.|.|X|.|\n"
        ".|.|.|X|X|.|.|\n"
        ".|.|.|X|.|.|.|", 'X');
    REQUIRE(Bitboard::print(Bitboard::line4(board), 'X') ==
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|X|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|X|.|.|.|\n"
            ".|.|.|.|.|.|.|");

    board = Bitboard::parse(
        "X|.|.|.|.|X|.|\n"
        ".|X|.|.|.|X|.|\n"
        ".|.|X|.|.|.|.|\n"
        ".|.|.|.|.|.|.|\n"
        ".|X|.|.|X|.|X|\n"
        "X|X|.|.|X|.|X|", 'X');
    REQUIRE(Bitboard::print(Bitboard::line4(board), 'X') ==
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|");
}
