#include "bench_connect4.hpp"

#include "connect4.hpp"
#include "timer.hpp"

Timer solveGame(Config config) {
    Connect4 solver(config);
    GameState board = GameState::random(0);

    Timer timer;
    timer.start();
    solver.solve(board);
    timer.stop();

    return timer;
}

void cacheSize() {
    Config config;
    config.width = 6;
    config.height = 5;
    std::cout << config << std::endl;

    config.useCache = false;
    std::cout << "cacheSize.0KB: " << solveGame(config) << std::endl;

    config.useCache = true;
    for(unsigned size=4; size<1024; size*=2) {
        config.cacheSize = size*Cache::Kilobytes;
        std::cout << "cacheSize." << size << "KB: " << solveGame(config) << std::endl;
    }
    for(unsigned size=1; size<1024; size*=2) {
        config.cacheSize = size*Cache::Megabytes;
        std::cout << "cacheSize." << size << "MB: " << solveGame(config) << std::endl;
    }

}

void benchConnect4() {

    
    Config config;
    config.width = 6;
    config.height = 5;
    config.cacheSize = 4*Cache::Megabytes;
    std::cout << config << std::endl;
    std::cout << "solve: " << solveGame(config) << std::endl;
    
    cacheSize();


/*    

    solveGame(config);

    {
        Config conf(config);
        conf.cacheSize = 256*Cache::Megabytes;
        solveGame(conf);
    }

    {
        Config conf(config);
        conf.cacheMaxDepth = 20;
        solveGame(conf);
    }

    {
        Config conf(config);
        conf.reorderMoves = false;
        solveGame(conf);
    }

    {
        Config conf(config);
        conf.moveFilter = false;
        solveGame(conf);
    }

    {
        Config conf(config);
        conf.cacheProbe = 256;
        solveGame(conf);
    }

    {
        Config conf(config);
        conf.useCache = false;
        solveGame(conf);
    }
*/
}
