#ifndef GAME
#define GAME

#include <cstdint>

#define PLAYER_INVALID 0
#define PLAYER_MAX 1
#define PLAYER_MIN 2

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

uint64 randUint64();

#define DEPTH_MAX 50

#endif
