#include "connect4.hpp"
#include <ctime>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>
#include <sstream>

#define MB 131072

bool terminate = false;

void printStats(const Connect4& game, double duration) {
    const Stats& stats = game.getStats();
    clear();
    printw("Duration: %.2f\n\n", duration);
    printw("%e nodes explored\n", (double)stats.nodesExplored);
    printw("%e nodes explored/s\n", (double)stats.nodesExplored/duration);
    printw("%e cutoffs\n", (double)stats.cutoffs);
    printw("%f average cutoff index\n", (double)stats.averageCutoff);

    printw("\nCache statistics:\n");
    double hitRate = float(stats.cacheHits*100)/float(stats.cacheHits+stats.cacheMisses);
    printw("Hitrate:\t%.2f%%\n", hitRate);

    printw(game.getCacheStats().c_str());
    printw("\n");
    refresh();
}

void* statsLoop(void *arg){
    Connect4* game = (Connect4*)arg;
    std::clock_t start = std::clock();

    initscr();
    double duration;
    while(!terminate) {
        duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
        printStats(*game, duration);
        sleep(1);
    }

    duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    printStats(*game, duration);
    printw("Press any key to exit\n");
    getch();

    endwin();

    std::cout << "Duration: " << duration << std::endl;

    return NULL;
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        std::cerr << "Usage: noob [startDepth]" << std::endl;
        exit(-1);
    }
    Depth depth = static_cast<Depth>(std::atoi(argv[1]));

    Connect4 game(2*MB);

    GameState state = GameState::random(depth);

    std::cout << "Solving:" << std::endl;
    std::cout << state.print() << std::endl;

    pthread_t statsThread;
    pthread_create(&statsThread, NULL, statsLoop, (void*)&game);

    Value value = game.solve(state);

    terminate = true;
    pthread_join(statsThread, NULL);

    std::cout << "Value=" << printValue(value) << std::endl;
/*
    {
        // Recalculate
        game.resetStats();
        game.clearCacheValues();
        terminate=false;
        pthread_t statsThread;
        pthread_create(&statsThread, NULL, statsLoop, (void*)&game);
        Value value = game.solve(state);
        terminate = true;
        pthread_join(statsThread, NULL);
        std::cout << "Value=" << printValue(value) << std::endl;
    }
*/
    return 0;
}
