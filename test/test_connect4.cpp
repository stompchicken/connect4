#include "catch.hpp"
#include "connect4.hpp"

#include <vector>

Value minimax(const GameState& board, std::vector<GameState>& moves, int maxDepth) {
    Value value = board.evaluate();
    if(value != VALUE_UNKNOWN) {
        moves.push_back(board);
        return value;
    } else if(board.getDepth() >= maxDepth) {
        moves.push_back(board);
        return board.heuristic();
    } else {
        GameState buffer[WIDTH];
        std::vector<GameState> submoves;
        board.children(buffer);

        value = (board.getPlayer() == PLAYER_MAX) ? VALUE_MIN : VALUE_MAX;
        for(int i=0; i<WIDTH; i++) {
            if(buffer[i].isValid()) {
                Value val = minimax(buffer[i], submoves, maxDepth);
                if((board.getPlayer() == PLAYER_MAX && val >= value) ||
                   (board.getPlayer() == PLAYER_MIN && val <= value)) {

                    value = val;
                    moves.clear();
                    for(std::vector<GameState>::iterator it=submoves.begin(); it!=submoves.end(); ++it) {
                        moves.push_back(*it);
                    }
                }
            }
        }

        moves.push_back(board);

        return value;
    }
}

TEST_CASE("Connect4::regression", "[fast]") {
    Connect4 game(20);

    GameState board = GameState::parse(
        ".|.|.|.|.|O|\n"
        "O|.|.|.|X|X|\n"
        "X|.|O|.|O|X|\n"
        "O|.|X|.|X|O|\n"
        "X|O|O|X|X|O|\n",
        PLAYER_MAX, 18);

    int maxDepth = 40;
    std::vector<GameState> moves;
    Value minimaxValue = minimax(board, moves, maxDepth);
    Value alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, maxDepth);
    INFO(board.print());

    std::cout << moves.size() << std::endl;
    for(std::vector<GameState>::reverse_iterator it=moves.rbegin(); it!=moves.rend(); ++it) {
        std::cout << "Move: \n" << it->print() << std::endl;
    }


    REQUIRE(minimaxValue == alphaBetaValue);
}

/*
TEST_CASE("Connect4::alphaBeta", "[slow][hide]") {
    for(int i=0; i<10000; i++) {
        Connect4 game(16);

#if WIDTH == 6 && HEIGHT == 5
        int startDepth = 18;
        int maxDepth = 30;
#elif WIDTH == 7 && HEIGHT == 6
        int startDepth = 32;
        int maxDepth = 42;
#endif

        GameState board = GameState::random(startDepth);

        Value minimaxValue = minimax(board, maxDepth);
        Value alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, maxDepth);

        INFO(board.print());
        REQUIRE(minimaxValue == alphaBetaValue);
    }
}
*/
