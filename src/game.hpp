#ifndef GAME
#define GAME

#include <stdint.h>

#define PLAYER_INVALID 0
#define PLAYER_MAX 1
#define PLAYER_MIN 2

#include <iostream>

int flipPlayer(int player);

typedef int Value;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint8_t uint8;

typedef uint8 Move;
typedef uint8 Depth;

const static Value VALUE_UNKNOWN = 0;
const static Value VALUE_MIN = 1;
const static Value VALUE_MAX = 255;
const static Value VALUE_DRAW = 128;

const static Move MOVE_INVALID = 7;

Value flipValue(Value value);

#define DEPTH_MAX 50

#endif
