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

                if(depth == 18) {
//                    std::cout << buffer[i].print() << std::endl;
//                    std::cout << "Value=" << printValue(val) << std::endl;

                }


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
/*
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
        std::cout << "Player: " << it->getPlayer() << std::endl;
        std::cout << "Value: " << it->evaluate() << std::endl;
    }


    REQUIRE(minimaxValue == alphaBetaValue);
}
*/
/*
TEST_CASE("Connect4::regression", "[fast]") {
    Connect4 game(20);

    GameState board;
    Value alphaBetaValue;

    board = GameState::parse(
        "X|.|.|.|.|O|\n"
        "O|.|.|.|X|X|\n"
        "X|.|O|.|O|X|\n"
        "O|.|X|.|X|O|\n"
        "X|O|O|X|X|O|\n");
    // depth = 19
    // okay up to depth 22

    unsigned pv[DEPTH_MAX];
    alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, pv);
    std::cout << board.print() << std::endl;
    std::cout << "AB value: " << static_cast<int>(alphaBetaValue) << std::endl;
    std::cout << game.getStats() << std::endl;
    for(int i=board.getDepth(); i<30; i++) {
        std::cout << "Depth=" << i << " -> " << pv[i] << std::endl;
    }

    Value minimaxValue = minimax(board, pv);
    std::cout << "MM value: " << static_cast<int>(minimaxValue) << std::endl;
    for(int i=board.getDepth(); i<30; i++) {
        std::cout << "Depth=" << i << " -> " << pv[i] << std::endl;
    }


    game.resetStats();


    board = GameState::parse(
        ".|.|.|.|.|O|\n"
        "O|.|X|.|X|X|\n"
        "X|.|O|.|O|X|\n"
        "O|.|X|.|X|O|\n"
        "X|O|O|X|X|O|\n");

    alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, pv);
    std::cout << board.print() << std::endl;
    std::cout << "AB value: " << static_cast<int>(alphaBetaValue) << std::endl;
    std::cout << game.getStats() << std::endl;

    minimaxValue = minimax(board, pv);
    std::cout << "MM value: " << static_cast<int>(minimaxValue) << std::endl;
    for(int i=board.getDepth(); i<30; i++) {
        std::cout << "Depth=" << i << " -> " << pv[i] << std::endl;
    }



    REQUIRE(false);
}
*/

/*
TEST_CASE("Connect4::alphaBeta", "[fast]") {

    Connect4 game(16);

    GameState board = GameState::parse(
        "X|.|.|X|.|.|\n"
        "O|.|.|O|.|.|\n"
        "X|O|.|X|X|.|\n"
        "O|X|X|O|O|.|\n"
        "X|O|O|X|O|X|\n");
    // Winning line: [1, 1, 2]
    unsigned moves[DEPTH_MAX];


    REQUIRE(game.alphaBeta(board, VALUE_MIN, VALUE_MAX, moves) == VALUE_MIN);
}
*/

void solve(Connect4& game, GameState state) {

    std::cout << "------------------------------" << std::endl;
    std::cout << "Solving: " << std::endl;
    std::cout << state.print() << std::endl;
    std::cout << "Depth: " << static_cast<int>(state.getDepth()) << std::endl;
    std::cout << "Player: " << printPlayer(state.getPlayer()) << std::endl;
    std::cout << std::endl;
    unsigned abpv[DEPTH_MAX];
    Value alphaBetaValue = game.alphaBeta(state, VALUE_MIN, VALUE_MAX, abpv);
    std::cout << std::endl;
    std::cout << "alphaBeta = " << printValue(alphaBetaValue) << std::endl;

    std::cout << "moves =  [";
    for(int i=state.getDepth(); i<30; i++) {
        std::cout << abpv[i] << ", ";
    }
    std::cout << "]" << std::endl;


    unsigned mmpv[DEPTH_MAX];
    Value minimaxValue = minimax(state, mmpv);
    std::cout << "minimax = " << printValue(minimaxValue) << std::endl;
    std::cout << "moves =  [";
    for(int i=state.getDepth(); i<30; i++) {
        std::cout << mmpv[i] << ", ";
    }
    std::cout << "]" << std::endl;


    std::cout << game.getStats() << std::endl;
    std::cout << "-----------------------------------------" << std::endl;


}


TEST_CASE("Connect4::bug", "[fast]") {
    Connect4 game(10);

    GameState board;


    board = GameState::parse(
        ".|X|.|.|O|.|\n"
        ".|O|O|.|X|.|\n"
        "O|O|X|.|X|.|\n"
        "X|O|X|.|O|.|\n"
        "O|X|O|X|X|.|\n");

    solve(game, board);

}


TEST_CASE("Connect4::compareMinimax", "[slow][hide]") {
    for(int i=0; i<10000; i++) {
        Connect4 game(16);

#if WIDTH == 6 && HEIGHT == 5
        int startDepth = 18;
#elif WIDTH == 7 && HEIGHT == 6
        int startDepth = 32;
        int maxDepth = 42;
#endif

        GameState board = GameState::random(startDepth);

        unsigned mmpv[DEPTH_MAX];
        Value minimaxValue = minimax(board, mmpv);
        unsigned abpv[DEPTH_MAX];
        Value alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, abpv);

        INFO(board.print());
        REQUIRE(minimaxValue == alphaBetaValue);

    }
}

