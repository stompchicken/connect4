#include "cache.hpp"

#include <gtest/gtest.h>
#include <cstdint>
#include <random>

using namespace testing;

class CacheTest : public Test {

    virtual void SetUp() {
        std::random_device rseed;
        gen.reset(std::mt199936(rseed()));
        uniform64();
        uniform8();
    }

    uint8_t genUint8() {
        return uniform
    }

  private:
    std::unique_ptr<std::mt19937> gen;
    std::uniform_int_distribution<uint64_t> uniform64;
    std::uniform_int_distribution<uint8_t> uniform8;

};

CacheValue randomCacheValue() {
    CacheValue val;
    val.lower = uniform8();
    val.upper = uniform8();
    val.move = uniform8();
    val.depth = uinform();
    return val;
}

TEST_F(CacheTest, GetPut) {
    Cache cache(16);
    for(int i=0; i<200; i++) {
        std::cout << i << std::endl;
        Connect4 board = Connect4::random();
        CacheValue val = randomCacheValue();
        ASSERT_TRUE(cache.put(board, val));
        CacheValue val2;
        ASSERT_TRUE(cache.get(board, val2)) << board;
        ASSERT_EQ(val, val2);
    }
}
