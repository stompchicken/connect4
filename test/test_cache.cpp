#include "catch.hpp"
#include "cache.hpp"
#include <iomanip>

TEST_CASE("Cache::packEntry", "[fast]") {
    Entry entry = { 0x1ffffffffffff, 42, 24};
    REQUIRE(unpackEntry(packEntry(entry)) == entry);
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

    CacheValue val1;
    val1.value = 54;
    val1.depth = 2;

    REQUIRE(cache.put(state, val1) == true);

    Entry val2;
    REQUIRE(cache.get(state, val2) == true);
    REQUIRE(val1 == val2);
}


TEST_CASE("Cache::randomAccess", "[slow][hide]") {
    Cache cache(16);

    for(int i=0; i < 10000; i++) {
        GameState state = GameState::random();
        Entry val1;
        val1.key = state.key();
        val1.value = rand() % 256;
        val1.depth = rand() % DEPTH_MAX;

        REQUIRE(cache.put(state, val1) == true);

        Entry val2;
        REQUIRE(cache.get(state, val2) == true);
        REQUIRE(val1 == val2);
    }
}


