#include "catch.hpp"
#include "cache.hpp"

CacheValue randomCacheValue() {
    CacheValue val;
    val.value = rand() % 256;
    val.depth = rand() % DEPTH_MAX;
    return val;
}

TEST_CASE("Cache works", "[fast]") {
/*    const int cacheSize = 16;
    const int n = 1 << (cacheSize - 6);
    Cache cache(cacheSize);
    for(int i=0; i < n; i++) {
        GameState board = GameState::random();

        CacheValue val1 = randomCacheValue();
        REQUIRE(cache.put(board, val1) == true);

        CacheValue val2;
        REQUIRE(cache.get(board, val2) == true);

        REQUIRE(val1 == val2);
    }
*/
}
