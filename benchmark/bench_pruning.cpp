#include <iostream>
#include <ctime>
#include <vector>

#include "connect4.hpp"
#include "cache.hpp"
#include "perfect.hpp"


void benchmark_pruning() {


    std::vector<Connect4> boards;

    boards.push_back(Connect4(22059759435905, 44152263917570, 1, 16));
    boards.push_back(Connect4(34632384535, 13263398076424, 1, 16));
    boards.push_back(Connect4(22093311852802, 43980735676549, 1, 16));
    boards.push_back(Connect4(71674364290, 4437779169793, 1, 16));
    boards.push_back(Connect4(4432945234562, 26388551730433, 1, 16));
    boards.push_back(Connect4(61572651206145, 4398854111616, 1, 16));
    boards.push_back(Connect4(206171029511, 13228501467272, 1, 16));
    boards.push_back(Connect4(4433484202625, 8865619936512, 1, 16));
    boards.push_back(Connect4(8796902539905, 57277685989632, 1, 16));
    boards.push_back(Connect4(8830729699329, 21990238847874, 1, 16));

    for(size_t i=0; i<boards.size(); i++) {
        Game game(24);
        auto start = std::clock();
        game.alphaBeta(boards[i], VALUE_MIN, VALUE_MAX, 42);
        auto end = std::clock();
        std::cout << (end - start) / (double) CLOCKS_PER_SEC << ",";
    }
    std::cout << std::endl;
}
