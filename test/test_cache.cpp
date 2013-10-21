#include "cache.hpp"
#include "game.hpp"

#include <gtest/gtest.h>
#include <random>

using namespace testing;

class CacheTest : public Test {
  protected:
    virtual void SetUp() {
//        random.reset(new Random());
//        random->seed(1234);
    }

    CacheValue randomCacheValue() {
        CacheValue val;
        val.lower = rand() % 256;
        val.upper = rand() % 256;
        val.move = rand() % WIDTH;
        val.depth = rand() % DEPTH_MAX;
        return val;
    }

  private:
//    std::unique_ptr<Random> random;

};


TEST_F(CacheTest, GetPut) {
    const int cacheSize = 16;
    const int n = 1 << (cacheSize - 6);
    Cache cache(cacheSize);
    for(int i=0; i < n; i++) {
        Connect4 board = Connect4::random();
        CacheValue val = randomCacheValue();
        ASSERT_TRUE(cache.put(board, val)) << i << std::endl << board;
        CacheValue val2;
        ASSERT_TRUE(cache.get(board, val2)) << i << std::endl << board;
        ASSERT_EQ(val, val2);
    }
}
