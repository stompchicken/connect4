#include <iostream>
#include <cstdlib>

#include "bench_state.hpp"
#include "bench_cache.hpp"
#include "bench_connect4.hpp"

int main() {
    srand(1234);

    std::cout << "========================================" << std::endl;
    std::cout << "Benchmarking state..." << std::endl;
    benchState();

    std::cout << "========================================" << std::endl;
    std::cout << "Benchmarking cache..." << std::endl;
    benchCache();

    std::cout << "========================================" << std::endl;
    std::cout << "Benchmarking solver..." << std::endl;
    benchConnect4();

}

