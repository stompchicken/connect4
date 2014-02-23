#include "catch.hpp"
#include "cache.hpp"

CacheValue randomCacheValue() {
    CacheValue val;
    val.value = rand() % 256;
    val.depth = rand() % DEPTH_MAX;
    return val;
}

TEST_CASE("Cache works", "[fast]") {
    const int cacheSize = 8;
    Cache cache(cacheSize);

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
    CacheValue val2;
    REQUIRE(cache.get(state, val2) == true);
    REQUIRE(val1 == val2);
}
