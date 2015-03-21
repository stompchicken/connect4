#include "cache.hpp"
#include "timer.hpp"

#include <vector>
#include <fstream>

// To avoid being optimised out
static Entry entry;
static Entry defaultEntry = {VALUE_MIN, VALUE_MAX, 0, MOVE_INVALID};

void walkTree(const GameState& node, unsigned width, unsigned height,
         Cache& cache, Timer& timer, size_t maxSamples) {
    timer.start();
    cache.get(node, entry);
    timer.stop();

    GameState children[WIDTH];
    node.children(children, width, height);

    for(unsigned i=0; i<WIDTH; i++) {
        if(timer.getSampleCount() > maxSamples) return;

        const GameState& child = children[i];
        if(!child.isValid()) continue;

        walkTree(child, width, height, cache, timer, maxSamples);
    }

    cache.put(node, defaultEntry);
}


void benchCache() {

    uint64 size = 4;
    uint64 entries = 1024*1024*8;

    std::cout << "Cache of " << size << " MBs" << std::endl;
    std::cout << "Put/Get " << entries << " entries" << std::endl;

    std::vector<uint64> probes;
    probes.push_back(2);
    probes.push_back(4);
    probes.push_back(8);
    probes.push_back(16);
    probes.push_back(32);
    probes.push_back(64);

    GameState state = GameState::random(16, 6, 5);

    for(unsigned i=0; i<probes.size(); i++) {
        uint64 probe = probes[i];
        Cache cache(size * Cache::Megabytes, probe);
        Timer timer;
        walkTree(state, 6, 5, cache, timer, entries);
        std::cout << "probe." << probe << ": " << timer << std::endl;
    }

}
