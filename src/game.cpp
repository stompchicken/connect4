#include "game.hpp"

#include <iostream>

int flipPlayer(int player) {
    return player == PLAYER_MAX ? PLAYER_MIN : PLAYER_MAX;
}

Value flipValue(Value value) {
    if(value == VALUE_UNKNOWN) return VALUE_UNKNOWN;
    return 256 - value;
}


