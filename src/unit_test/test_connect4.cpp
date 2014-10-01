#include "catch.hpp"
#include "connect4.hpp"

TEST_CASE("Connect4::alphaBeta") {

    Config config;
    config.cacheSize = 1*Cache::Megabytes;
    Connect4 game(config);

    {
        GameState board = GameState::parse(
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            "X|.|.|.|.|.|.|\n"
            "X|.|.|.|.|.|.|\n"
            "X|.|O|.|O|.|O|\n");
        // Winning line: [O->2]
        Move bestMove;
        REQUIRE(game.alphaBeta(board, VALUE_MIN, VALUE_MAX, bestMove) == VALUE_MAX);
        REQUIRE(bestMove == 0);

        Move pv[DEPTH_MAX];
        game.principleVariation(board, pv);
        REQUIRE(pv[6] == 0);
        REQUIRE(pv[7] == MOVE_INVALID);
    }

    game.reset();

    {
        GameState board = GameState::parse(
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|.|.|.|.|.|.|\n"
            ".|X|O|.|.|.|.|\n"
            ".|O|X|O|X|.|.|\n"
            "X|X|O|O|O|.|X|\n");
        Move bestMove;
        REQUIRE(game.alphaBeta(board, VALUE_MIN, VALUE_MAX, bestMove) == VALUE_MIN);
//        REQUIRE(bestMove == 1);

        Move pv[DEPTH_MAX];
        game.principleVariation(board, pv);
//        REQUIRE(pv[13] == 1);
//        REQUIRE(pv[15] == );
        REQUIRE(pv[16] == MOVE_INVALID);
    }
}

