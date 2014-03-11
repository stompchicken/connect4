#include "catch.hpp"
#include "cache.hpp"
#include <iomanip>
#include <bitset>

TEST_CASE("Cache::packEntry", "[fast]") {
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

TEST_CASE("Cache::put", "[fast]") {
    Cache cache(8);

    GameState state = GameState();
    state.parse(".|O|.|.|.|.|.|\n"
                ".|X|.|.|.|.|.|\n"
                ".|O|.|.|.|.|.|\n"
                ".|X|.|.|.|.|.|\n"
                ".|X|.|X|.|O|.|\n"
                ".|X|O|O|X|O|X|");

    Entry e1;
    e1.lower = VALUE_MIN;
    e1.upper = VALUE_DRAW;
    e1.depth = 2;
    e1.bestMove = 1;

    REQUIRE(cache.put(state, e1) == true);

    Entry e2;
    REQUIRE(cache.get(state, e2) == true);
    REQUIRE(e1 == e2);
}


TEST_CASE("Cache::randomAccess", "[slow][hide]") {
    Cache cache(16);

    for(int i=0; i < 10000; i++) {
        GameState state = GameState::random();
        Entry val1;

        Value v1 = (rand() % 3) + 1;
        Value v2 = (rand() % 3) + 1;
        val1.lower = v1 < v2 ? v1 : v2;
        val1.upper = v1 > v2 ? v1 : v2;
        val1.depth = rand() % DEPTH_MAX;
        val1.bestMove = rand() % 8;

        REQUIRE(cache.put(state, val1) == true);

        Entry val2;
        REQUIRE(cache.get(state, val2) == true);
        REQUIRE(val1 == val2);
    }
}
