#include "connect4.hpp"
#include <ctime>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>
#include <sstream>

#define MB 131072

bool terminate = false;

void* printStats(void *arg){
    Connect4* game = (Connect4*)arg;


    std::clock_t start = std::clock();

    initscr();
    double duration;

    while(!terminate) {
        clear();

        duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;

        const Stats& stats = game->getStats();

        printw("Duration: %.2f\n\n", duration);
        printw("%e nodes explored\n", (double)stats.nodesExplored);
        printw("%e nodes explored/s\n", (double)stats.nodesExplored/duration);

        printw("\nCache statistics:\n");
        printw(game->getCacheStats().c_str());
        printw("\n");

        refresh();
        sleep(1);
    }


    printw("Press any key to exit\n");
    getch();

    endwin();

    std::clock_t end = std::clock();
    std::cout << "Duration: " << (end - start) / (double) CLOCKS_PER_SEC << std::endl;

    return NULL;
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        std::cerr << "Usage: noob [startDepth]" << std::endl;
        exit(-1);
    }
    Depth depth = static_cast<Depth>(std::atoi(argv[1]));

    Connect4 game(2048*MB);


    GameState board = GameState::random(depth);
    std::cout << "Solving:" << std::endl;
    std::cout << board.print() << std::endl;

    pthread_t statsThread;
    pthread_create(&statsThread, NULL, printStats, (void*)&game);

    Value value = game.solve(board);

    terminate = true;
    pthread_join(statsThread, NULL);

    std::cout << "Value: " << printValue(value) << std::endl;


//    game.resetStats();
/*
    game.clearCacheValues();
    std::cout << "Clearing cache" << std::endl;
    std::cout << board.print() << std::endl;
    start = std::clock();
    value = game.alphaBeta(board, VALUE_MIN, VALUE_MAX);
    end = std::clock();
    std::cout << "Duration: " << (end - start) / (double) CLOCKS_PER_SEC << std::endl;
    std::cout << game.getStats();
    game.printCacheStats();
*/


    return 0;
}
