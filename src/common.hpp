#ifndef COMMON
#define COMMON

#define WIDTH  7
#define HEIGHT 6
#define SIZE   WIDTH*(HEIGHT+1)

#include <stdint.h>

typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint8_t uint8;

typedef uint8 Player;
const static Player PLAYER_INVALID = 0;
const static Player PLAYER_MAX = 1;
const static Player PLAYER_MIN = 2;

typedef uint8 Value;
const static Value VALUE_UNKNOWN = 0;
const static Value VALUE_MIN = 1;
const static Value VALUE_DRAW = 2;
const static Value VALUE_MAX = 3;

typedef uint8 Move;
const static Move MOVE_INVALID = 255;

typedef uint8 Depth;
const static Depth DEPTH_MAX = WIDTH*HEIGHT;


#endif
