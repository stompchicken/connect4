#ifndef NAIVESTATE
#define NAIVESTATE

#include "common.hpp"
#include "gameState.hpp"

class NaiveState {

  public:
    NaiveState() {}

    void random(Depth) {
    }

    Value evaluate() const {
        return VALUE_UNKNOWN;
    }

    void children(NaiveState* buffer, size_t size) const {
        for(size_t i=0; i<size; i++) {
            for(unsigned j=0; j<DEPTH_MAX; j++) {
                buffer[i].cells[j] = cells[j];
            }
        }
    }

private:
    bool cells[DEPTH_MAX];
};

#endif
