#include "cache.hpp"
#include "timer.hpp"

Timer putGet(Cache& cache, uint64 entries) {

    Timer timer;

    Entry cacheEntry = {VALUE_MIN, VALUE_MAX, 0, MOVE_INVALID};
    for(uint64 i=0; i<entries; i++) {
        Depth depth = rand() % 48;
        GameState state = GameState::random(depth);
        cacheEntry.depth = depth;
        timer.start();
        cache.put(state, cacheEntry);
        timer.stop();
    }

    for(uint64 i=0; i<entries; i++) {
        Depth depth = rand() % 48;
        GameState state = GameState::random(depth);
        cacheEntry.depth = depth;
        timer.start();
        cache.get(state, cacheEntry);
        timer.stop();
    }
    
    return timer;
}

void probe() {

    uint64 size = 4*Cache::Megabytes;
    uint64 entries = size/2;

    std::cout << "Cache of " << size << " bytes" << std::endl;
    std::cout << "Put/Get " << entries << " entries" << std::endl;

    for(unsigned probe=4; probe<1024; probe*=2) {
        Cache cache(size, probe);
        std::cout << "probe." << probe << ": " << putGet(cache, entries) << std::endl;
    }
}

void benchCache() {

    {
        uint64 size = 4*Cache::Megabytes;
        Cache cache(size);
        uint64 entries = size;
        std::cout << "Cache of " << size << " bytes" << std::endl;
        std::cout << "Put/Get " << entries << " entries" << std::endl;
        std::cout << "putGet: " << putGet(cache, entries) << std::endl;
    }

    probe();
}
