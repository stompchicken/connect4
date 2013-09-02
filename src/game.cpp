#include "game.hpp"

#include <iostream>

int flipPlayer(int player) {
    return player == PLAYER_MAX ? PLAYER_MIN : PLAYER_MAX;
}

Value flipValue(Value value) {
    if(value == VALUE_UNKNOWN) return VALUE_UNKNOWN;
    return 256 - value;
}

uint64 randUint64() {
    return rand();
    //uint64 x = static_cast<uint64>(rand()) << 32;
    //return x | static_cast<uint64>(rand());
}
