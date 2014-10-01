#ifndef COMMON
#define COMMON

#include <stdint.h>
#include <string>
#include <iostream>

#define WIDTH  7
#define HEIGHT 6
#define SIZE   WIDTH*(HEIGHT+1)

typedef int64_t int64;
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
const static Move MOVE_INVALID = WIDTH;

typedef uint8 Depth;
const static Depth DEPTH_MAX = WIDTH*HEIGHT;

inline Player flipPlayer(Player player) {
    return player == PLAYER_MAX ? PLAYER_MIN : PLAYER_MAX;
}

inline std::string printPlayer(Player player) {
    return player == PLAYER_MAX ? "PLAYER_MAX" : "PLAYER_MIN";
}

inline Value flipValue(Value value) {
    if(value == VALUE_UNKNOWN) return VALUE_UNKNOWN;
    else if(value == VALUE_MAX) return VALUE_MIN;
    else if(value == VALUE_MIN) return VALUE_MAX;
    else return VALUE_DRAW;
}

inline std::string printValue(Value value) {
    if(value == VALUE_UNKNOWN) return "VALUE_UNKNOWN";
    else if(value == VALUE_MAX) return "VALUE_MAX";
    else if(value == VALUE_MIN) return "VALUE_MIN";
    else return "VALUE_DRAW";

}

#endif
