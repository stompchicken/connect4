#include "connect4.hpp"
#include <ctime>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>
#include <sstream>

bool terminate = false;

void* statsLoop(void *arg);
void printStats(const Connect4& game, double duration);

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

    if(argc != 3 && argc != 5 ) {
        std::cerr << "Usage: connect4 [startDepth] [cacheSize(MB)] [width=7] [height=6]" << std::endl;
        exit(-1);
    }
    Depth depth = static_cast<Depth>(std::atoi(argv[1]));
    size_t cacheSize = static_cast<size_t>(std::atoi(argv[2])) * Cache::Megabytes;

    Config config;
    config.cacheSize = cacheSize;

    if(argc == 5) {
        unsigned width = static_cast<unsigned>(std::atoi(argv[3]));
        unsigned height = static_cast<unsigned>(std::atoi(argv[4]));
        config.width = width;
        config.height = height;
    }

    std::cout << config << std::endl;
    Connect4 game(config);

    GameState state = GameState::random(depth);

    std::cout << "Solving:" << std::endl;
    std::cout << state.print() << std::endl;

    // Kick off a stats reporting thread
    pthread_t statsThread;
    pthread_create(&statsThread, NULL, statsLoop, (void*)&game);

    // Solve the game
    Value value = game.solve(state);

    // Terminate stats thread
    terminate = true;
    pthread_join(statsThread, NULL);

    std::cout << "Value=" << printValue(value) << std::endl;

    return 0;
}
