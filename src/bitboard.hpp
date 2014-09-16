#ifndef BITBOARD
#define BITBOARD

#include <string>

#include "common.hpp"

namespace Bitboard {

typedef uint64 Board;

/*

Bitboard structure, column major with zero barrier at max height

X = P1 (maximising player)
O = P2 (minimising player)

     0  1  2  3  4  5  6
   |XX|XX|XX|XX|XX|XX|XX|
5  |05|12|19|26|33|40|47|
4  |04|11|18|25|32|39|46|
3  |03|10|17|24|31|38|45|
2  |02|09|16|23|30|37|44|
1  |01|08|15|22|29|36|43|
0  |00|07|14|21|28|35|42|

*/

// Conversions from row,column to index
inline unsigned toIndex(unsigned row, unsigned col) {
    return (col * (HEIGHT+1)) + row;
}

inline uint64 toMask(unsigned row, unsigned col) {
    return (uint64)1 << toIndex(row, col);
}

inline unsigned toRow(unsigned index) {
    return index % (HEIGHT+1);
}

inline unsigned toCol(unsigned index) {
    return index / (HEIGHT+1);
}

uint64 makeZeroBarrier();
uint64 makeBaseBarrier();

// Has a bit set for every valid piece location
const static uint64 zeroBarrier = makeZeroBarrier();

// Has a bit set in the bottom row
// Used to calculate a unique hash of two bitboards
const static uint64 baseBarrier = makeBaseBarrier();

// Apparently pretty fast?
// TODO: Add comment where I got this
// Could be replaced by an op on newer hardware
unsigned popcount(uint64 x);

// Mirrors a board around the centre vertical
Board flip(Board x);

// To/from string
Board parse(const std::string& text, char piece);
std::string print(Board b, char piece);

// Count lines of a given length
// Returns 64-bit int with a 1 for each line
uint64 line4(Board b);
uint64 line3(Board b);
uint64 line2(Board b);

}

#endif
