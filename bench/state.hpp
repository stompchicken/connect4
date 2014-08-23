#ifndef STATE
#define STATE

#include "common.hpp"

class NaiveState {

  public:
    NaiveState() {}

    void random(Depth depth) {
    }

    Value evaluate() const {
        return VALUE_UNKNOWN;
    }

    void children(NaiveState* buffer) const {
    }
};

class BitboardState {

  public:
    BitboardState() {}

    void random(Depth depth) {
    }

    Value evaluate() const {
        return VALUE_UNKNOWN;
    }

    void children(BitboardState* buffer) const {
    }
};


#endif
