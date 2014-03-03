#include "catch.hpp"
#include "cache.hpp"
#include <iomanip>

TEST_CASE("Cache::packEntry", "[fast]") {
    Key k1, k2;
    Entry e1, e2;
    Packed p1, p2;

    e1.value = 42;
    e1.depth = 24;
    k1 = 123456789;
    packEntry(k1, e1, p1);
    unpackEntry(p1, k2, e2);

    REQUIRE(k1 == k2);
    REQUIRE(e1 == e2);

    p1 = 123456789123456789;
    unpackEntry(p1, k1, e1);
    packEntry(k1, e1, p2);

    REQUIRE(p1 == p2);
}

TEST_CASE("Cache::put", "[fast]") {
    Cache cache(8);

    GameState state = GameState();
    state.parse(".|.|.|.|.|.|.|\n"
                ".|X|.|X|.|.|.|\n"
                ".|.|.|.|.|.|.|\n"
                ".|X|.|.|O|.|.|\n"
                ".|.|.|.|.|.|.|\n"
                ".|.|O|.|.|O|.|",
                PLAYER_MAX, 0);

    Entry e1;
    e1.value = 54;
    e1.depth = 2;

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
        val1.value = rand() % 256;
        val1.depth = rand() % DEPTH_MAX;

        REQUIRE(cache.put(state, val1) == true);

        Entry val2;
        REQUIRE(cache.get(state, val2) == true);
        REQUIRE(val1 == val2);
    }
}
