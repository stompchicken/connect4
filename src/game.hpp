#ifndef GAME
#define GAME

#include <cstdint>
#include <random>

#define PLAYER_INVALID 0
#define PLAYER_MAX 1
#define PLAYER_MIN 2

#include <iostream>

int flipPlayer(int player);

typedef int Value;
typedef uint64_t uint64;
typedef uint8_t Move;

const static Value VALUE_UNKNOWN = 0;
const static Value VALUE_MIN = 1;
const static Value VALUE_MAX = 255;
const static Value VALUE_DRAW = 128;

const static Move MOVE_INVALID = 7;

Value flipValue(Value value);

class Random {

  public:
    Random() {
        randomDevice.reset(new std::random_device());
        generator.reset(new std::mt19937((*randomDevice)()));
    }

    Random(uint64 s) {
        randomDevice.reset(new std::random_device());
        generator.reset(new std::mt19937((*randomDevice)()));
        seed(s);
    }

    void seed(uint64 seed) {
        generator->seed(seed);
    }

    uint8_t gen_uint8() {
        return uniform8(*generator);
    }

    uint64_t gen_uint64() {
        return uniform64(*generator);
    }

  private:
    std::unique_ptr<std::random_device> randomDevice;
    std::unique_ptr<std::mt19937> generator;
    std::uniform_int_distribution<uint64_t> uniform64;
    std::uniform_int_distribution<uint8_t> uniform8;
};

#define DEPTH_MAX 50

#endif
