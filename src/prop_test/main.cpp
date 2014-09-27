#include "connect4.hpp"
/*
Value minimax(const GameState& board, Move& bestMove, unsigned width, unsigned height) {
    Depth depth = board.getDepth();
    Value value = board.evaluate();
    bestMove = MOVE_INVALID;

#ifdef DEBUG
    assert(board.isValid());
    assert(depth <= DEPTH_MAX);
#endif

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

        return value;
    }
}
*/
GameState reduce(GameState board, unsigned width, unsigned height) {

    Connect4 game(4*Cache::Megabytes);
    game.setBoardSize(width, height);

    Value minimaxValue = game.minimax(board);

    Move alphaBetaMove;
    Value alphaBetaValue = game.alphaBeta(board, VALUE_MIN, VALUE_MAX, alphaBetaMove);

    GameState state = board;

    for(Depth depth = board.getDepth(); depth < DEPTH_MAX; depth++) {
        bool error = false;
        GameState buffer[WIDTH];
        state.children(buffer, width, height);
        for(Move move=0; move<WIDTH; move++) {
            if(!buffer[move].isValid()) continue;
        
            minimaxValue = game.minimax(buffer[move]);
            alphaBetaValue = game.solve(buffer[move]);

            if(minimaxValue != alphaBetaValue) {
                state = buffer[move];
                error = true;
                break;
            }

        }

        if(!error) break;
    }
    
    return state;
}

bool minimaxTest(unsigned runs, Depth depth, unsigned width, unsigned height) {
    Connect4 game(64*Cache::Megabytes);
    game.setBoardSize(width, height);

    for(unsigned i=0; i<runs; i++) {
        GameState board = GameState::random(depth, width, height);
        Value minimaxValue = game.minimax(board);
        Value alphaBetaValue = game.solve(board);

        if(minimaxValue != alphaBetaValue) {
            std::cerr << std::endl << "Alpha-beta and minimax don't agree!" << std::endl;
            std::cerr << board << std::endl;
            std::cerr << board.print() << std::endl;
            std::cout << "Minimax value=" << printValue(minimaxValue) << std::endl;
            std::cout << "Alpha-beta value=" << printValue(alphaBetaValue) << std::endl;

            GameState reduced = reduce(board, width, height);
            std::cerr << "Reduced to..." << std::endl;
            std::cerr << reduced << std::endl;
            std::cerr << reduced.print() << std::endl;
            minimaxValue = game.minimax(board);
            alphaBetaValue = game.solve(board);
            std::cout << "Minimax value=" << printValue(minimaxValue) << std::endl;
            std::cout << "Alpha-beta value=" << printValue(alphaBetaValue) << std::endl;

//            return false;
        }

        std::cout << "." << std::flush;
        if((i+1) % 80 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;

    return true;
}

void test() {

    GameState board(805421185, 3223453958, 1, 14);

    std::cout << "Solving..." <<  std::endl;
    std::cout << board.print() << std::endl;

    unsigned width = 5;
    unsigned height = 4;

    Connect4 game(64*Cache::Megabytes);
    game.setBoardSize(width, height);

    Value minimaxValue = game.minimax(board);
    Value alphaBetaValue = game.solve(board);

    std::cout << "-----------------------" << std::endl;
    std::cout << "Minimax=" << printValue(minimaxValue) << std::endl;
    std::cout << "alphaBeta=" << printValue(alphaBetaValue) << std::endl;
}

int main() {
    srand(1234);
    bool return_val = true;

    int n=10000;
    std::cout << "7x6" << std::endl;
    return_val &= minimaxTest(n, 28, 7, 6);
    std::cout << "6x5" << std::endl;
    return_val &= minimaxTest(n, 16, 6, 5);
    std::cout << "5x4" << std::endl;
    return_val &= minimaxTest(n, 8, 5, 4);


    std::cout << std::endl << (return_val ? "All passed" : "Failed") << std::endl;
}

