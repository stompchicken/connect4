#include "catch.hpp"
#include "cache.hpp"

#include <iomanip>
#include <bitset>

TEST_CASE("Cache::packEntry") {
    Key k1, k2;
    Entry e1, e2;
    Packed p1;

    e1.lower = VALUE_MIN;
    e1.upper = VALUE_MAX;
    e1.depth = 12;
    e1.bestMove = 2;
    k1 = 12345678912345;
    packEntry(k1, e1, p1);
    unpackEntry(p1, k2, e2);

    REQUIRE(k1 == k2);
    REQUIRE(e1 == e2);
}

TEST_CASE("Cache::putThenGet") {
    Cache cache(256);

    Entry e1, e2;

    GameState state = GameState::parse(
        ".|.|.|.|.|.|\n"
        "X|.|.|.|.|.|\n"
        "O|.|.|.|.|.|\n"
        "X|.|.|.|.|.|\n"
        "X|.|X|.|O|.|\n"
        "X|O|O|X|O|X|");

    e1.lower = VALUE_MIN;
    e1.upper = VALUE_DRAW;
    e1.depth = 13;
    e1.bestMove = 1;

    REQUIRE(cache.put(state, e1) == true);
    REQUIRE(cache.get(state, e2) == true);
    REQUIRE(e1 == e2);
}

TEST_CASE("Cache::randomAccess") {
    Cache cache(64*1024);

    Entry e1, e2;
    for(int i=0; i < 10; i++) {
        // Generate random cache entry
        GameState state = GameState::random(16);
        Value v1 = (rand() % 3) + 1;
        Value v2 = (rand() % 3) + 1;
        e1.lower = v1 < v2 ? v1 : v2;
        e1.upper = v1 > v2 ? v1 : v2;
        e1.depth = state.getDepth();
        e1.bestMove = rand() % 8;

        INFO(state.print())
        REQUIRE(cache.put(state, e1) == true);
        REQUIRE(cache.get(state, e2) == true);
        REQUIRE(e1 == e2);
    }
}
