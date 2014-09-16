#include "bitboard.hpp"

#include <sstream>

namespace Bitboard {

uint64 makeZeroBarrier() {
    uint64 z = 0;
    for(unsigned row=0; row<HEIGHT; row++) {
        for(unsigned col=0; col<WIDTH; col++) {
            z |= toMask(row, col);
        }
    }
    return z;
}

uint64 makeBaseBarrier() {
    uint64 base = 0;
    for(unsigned col=0; col<WIDTH; col++) {
        base |= toMask(0, col);
    }
    return base;
}

// Apparently pretty fast?
// TODO: Add comment where I got this
// Could be replaced by an op on newer hardware
unsigned popcount(uint64 x) {
    uint8 c = 0;
    for (; x > 0; x &= x -1) c++;
    return c;
}

// Mirrors a board around the centre vertical
Board flip(Board x) {
    Board flip = Board(0);
    for(unsigned col=0; col<WIDTH; col++) {
        for(unsigned row=0; row<HEIGHT; row++) {
            if(x & toMask(row, col)) {
                flip |= toMask(row, WIDTH-col-1);
            }
        }
    }
    return flip;
}

Board parse(const std::string& text, char piece) {
    uint64 mask;
    unsigned col = 0;
    unsigned row = HEIGHT - 1;
    Board board = 0;
    for(unsigned i = 0; i < text.size(); i++) {
        mask = toMask(row, col);
        if(text[i] == '\n') {
            row -= 1;
            col = 0;
        } else if(text[i] == '|') {
            col += 1;
        } else if(text[i] == piece) {
            board |= mask;
        }
    }
    return board;
}

std::string print(Board b, char piece) {
    std::stringstream stream;
    uint64 mask;
    for(unsigned row=0; row<HEIGHT; row++) {
        for(unsigned col=0; col<WIDTH; col++) {
            mask = toMask((HEIGHT-1)-row, col);

            if(b & mask) {
                stream << piece << "|";
            } else {
                stream << ".|";
            }
        }
        if(row != HEIGHT - 1) {
            stream << "\n";
        }
    }
    return stream.str();
}


// Count lines of a given length
// Returns 64-bit int with a 1 for each line
uint64 line4(Board b) {
    uint64 x = 0;

    x = (b << 2) & b;
    uint64 vertical = x & (x << 1);

    x = (b << (2*(HEIGHT+1))) & b;
    uint64 horizontal = x & (x << (HEIGHT+1));

    x = (b << (2*(HEIGHT+2))) & b;
    uint64 diagRight = x & (x << (HEIGHT+2));

    x = (b << (2*(HEIGHT))) & b;
    uint64 diagLeft = x & (x << (HEIGHT));

    return (vertical | horizontal | diagLeft | diagRight) & zeroBarrier;
}

uint64 line3(Board b) {
    uint64 x = 0;
    unsigned s = 0;
    uint64 l1, l2, l3;
    uint64 r1, r2, r3;

    // Vertical
    s = 1;
    l1 = b << 1;
    l2 = b << 2;
    l3 = b << 3;
    r1 = b >> 1;
    r2 = b >> 2;
    r3 = b >> 3;

    x |= (~b & l1 & l2 & l3) | (~b & l1 & l2 & r1) |
         (~b & r1 & r2 & l1) | (~b & r1 & r2 & r3);

    // Horizontal
    s = HEIGHT+1;
    l1 = b << 1*s;
    l2 = b << 2*s;
    l3 = b << 3*s;
    r1 = b >> 1*s;
    r2 = b >> 2*s;
    r3 = b >> 3*s;

    x |= (~b & l1 & l2 & l3) | (~b & l1 & l2 & r1) |
         (~b & r1 & r2 & l1) | (~b & r1 & r2 & r3);

    // Left diagonal
    s = HEIGHT;
    l1 = b << 1*s;
    l2 = b << 2*s;
    l3 = b << 3*s;
    r1 = b >> 1*s;
    r2 = b >> 2*s;
    r3 = b >> 3*s;

    x |= (~b & l1 & l2 & l3) | (~b & l1 & l2 & r1) |
         (~b & r1 & r2 & l1) | (~b & r1 & r2 & r3);

    // Right diagonal
    s = HEIGHT+2;
    l1 = b << 1*s;
    l2 = b << 2*s;
    l3 = b << 3*s;
    r1 = b >> 1*s;
    r2 = b >> 2*s;
    r3 = b >> 3*s;

    x |= (~b & l1 & l2 & l3) | (~b & l1 & l2 & r1) |
         (~b & r1 & r2 & l1) | (~b & r1 & r2 & r3);

    return x & zeroBarrier;
}

uint64 line2(Board b) {
    uint64 x = 0;
    unsigned s = 1;
    x |= ~b & (b << (1*s)) & (b << (2*s)) & ~(b << (3*s));
    s = HEIGHT;
    x |= ~b & (b << (1*s)) & (b << (2*s)) & ~(b << (3*s));
    s = HEIGHT+1;
    x |= ~b & (b << (1*s)) & (b << (2*s)) & ~(b << (3*s));
    s = HEIGHT+2;
    x |= ~b & (b << (1*s)) & (b << (2*s)) & ~(b << (3*s));
    return x;
}
}
