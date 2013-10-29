#ifndef NOOB
#define NOOB

#include <stdint.h>
#include <cassert>

typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint8_t uint8;


typedef int Player;
const static Player PLAYER_INVALID = 0;
const static Player PLAYER_MAX = 1;
const static Player PLAYER_MIN = 2;
Player flipPlayer(Player player);

typedef int Value;
const static Value VALUE_UNKNOWN = 0;
const static Value VALUE_MIN = 1;
const static Value VALUE_MAX = 255;
const static Value VALUE_DRAW = 128;
Value flipValue(Value value);

typedef uint8 Move;
const static Move MOVE_INVALID = 7;

typedef uint8 Depth;
const static Depth DEPTH_MAX = 50;

#endif
