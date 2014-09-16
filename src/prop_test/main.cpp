#include "connect4.hpp"

Value minimax(const GameState& board, Move& bestMove, unsigned width, unsigned height) {
    Depth depth = board.getDepth();
    Value value = board.evaluate();
    bestMove = MOVE_INVALID;

#ifdef DEBUG
    assert(board.isValid());
    assert(depth <= DEPTH_MAX);
#endif


//    if(depth <= 31) { std::cout << board.print() << std::endl; }

    if(value != VALUE_UNKNOWN) {
        return value;
    } else if (depth == width*height) {
        return VALUE_DRAW;
    } else {
        GameState buffer[WIDTH];
        board.children(buffer, width, height);

        value = VALUE_UNKNOWN;
        for(int i=0; i<WIDTH; i++) {
            if(buffer[i].isValid()) {
                Move childBestMove;
                Value val = minimax(buffer[i], childBestMove, width, height);

                if(value == VALUE_UNKNOWN || 
                   (board.getPlayer() == PLAYER_MAX && val > value) ||
                   (board.getPlayer() == PLAYER_MIN && val < value)) {
                    value = val;
                    bestMove = i;
                }
            }
        }

//        if(depth <= 31) { std::cout << "Value=" << printValue(value) << std::endl; }

        return value;
    }
}

GameState reduce(GameState board, unsigned width, unsigned height) {

    Connect4 game(4*Cache::Megabytes);
    game.setBoardSize(width, height);

    Move minimaxMove;
    Value minimaxValue = minimax(board, minimaxMove, width, height);

    Move alphaBetaMove;
    Value alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, alphaBetaMove);

//    std::cout << "Reducing " << std::endl << board.print() << std::endl;
//    std::cout << printValue(minimaxValue) << "(mm) != " << printValue(alphaBetaValue) << "(ab)" << std::endl;
    

    GameState state = board;

    for(Depth depth = board.getDepth(); depth < DEPTH_MAX; depth++) {
        bool error = false;
        GameState buffer[WIDTH];
        state.children(buffer, width, height);
        for(Move move=0; move<WIDTH; move++) {
            if(!buffer[move].isValid()) continue;
        
            minimaxValue = minimax(buffer[move], minimaxMove, width, height);
            alphaBetaValue = game.solve(buffer[move]);

            if(minimaxValue != alphaBetaValue) {
                std::cout << "Reduce -> " << std::endl << buffer[move].print() << std::endl;
                std::cout << printValue(minimaxValue) << "(mm) != " << printValue(alphaBetaValue) << "(ab)" << std::endl;
                state = buffer[move];
                error = true;
                break;
            }

        }

        if(!error) break;
    }

    std::cout << "Cannot reduce further" << std::endl;
    std::cout << "Player to move: " << printPlayer(state.getPlayer()) << std::endl;
    GameState buffer[WIDTH];
    state.children(buffer, width, height);
    for(Move move=0; move<WIDTH; move++) {

        if(!buffer[move].isValid()) continue;
        minimaxValue = minimax(buffer[move], minimaxMove, width, height);
        Connect4 game2(4*Cache::Megabytes);
        alphaBetaValue = game2.solve(buffer[move]);

        
        std::cout << "Move: " << (int)move << std::endl;
        std::cout << buffer[move].print() << std::endl;
        std::cout << buffer[move] << std::endl;
        std::cout << printValue(minimaxValue) << "(mm) == " << printValue(alphaBetaValue) << "(ab)" << std::endl;
    }
    
    return state;

}

bool minimaxTest(unsigned runs, Depth depth, unsigned width, unsigned height) {
    Connect4 game(1*Cache::Megabytes);
    for(unsigned i=0; i<runs; i++) {
//        Connect4 game(64*Cache::Megabytes);
        game.reset();
        game.setBoardSize(width, height);

        GameState board = GameState::random(depth, width, height);
        Move minimaxMove;
        Value minimaxValue = minimax(board, minimaxMove, width, height);

        Value alphaBetaValue = game.solve(board);

        if(minimaxValue != alphaBetaValue) {
            std::cerr << std::endl << "Alpha-beta and minimax don't agree!" << std::endl;
            std::cerr << board.print() << std::endl;
            std::cout << printValue(minimaxValue) << "(mm) != " << printValue(alphaBetaValue) << "(ab)" << std::endl;
            reduce(board, width, height);

            return false;
        }

        std::cout << "." << std::flush;
        if((i+1) % 80 == 0) std::cout << std::endl;

    }
    std::cout << std::endl;

    return true;
}

void testBoard() {

    Connect4 game(4*Cache::Megabytes);

    GameState board = GameState::parse(
        "O|.|.|.|.|X|.|\n"
        "O|X|.|.|O|X|.|\n"
        "X|X|.|.|X|O|X|\n"
        "X|O|.|.|X|O|O|\n"
        "O|O|O|X|X|O|X|\n"
        "X|O|O|X|O|X|O|");

    std::cout << board << std::endl;
    std::cout << board.print() << std::endl;

    std::cout << std::endl << std::endl;

    Value value = game.solve(board);
    std::cout << printValue(value) << std::endl;

    Move bestMove;
    value = minimax(board, bestMove, 7, 6);
//    std::cout << printValue(value) << std::endl;



}


int main() {
    srand(1234);
    bool return_val = true;

    testBoard();

    int n=0;
    std::cout << "7x6" << std::endl;
    return_val &= minimaxTest(n, 30, 7, 6);
    std::cout << "6x5" << std::endl;
    return_val &= minimaxTest(n, 20, 6, 5);
    std::cout << "5x4" << std::endl;
    return_val &= minimaxTest(n, 12, 5, 4);


    std::cout << std::endl << (return_val ? "All passed" : "Failed") << std::endl;
}

