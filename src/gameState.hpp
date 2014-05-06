#ifndef GAMESTATE
#define GAMESTATE

#include "common.hpp"

#include <iostream>
#include <cstdlib>
#include <cassert>


Player flipPlayer(Player player);
std::string printPlayer(Player player);
Value flipValue(Value value);
std::string printValue(Value value);


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

class Bitboard {
  public:
    inline static unsigned toIndex(unsigned row, unsigned col) {
        return (col * (HEIGHT+1)) + row;
    }

    inline static unsigned toRow(unsigned index) {
        return index % (HEIGHT+1);
    }

    inline static unsigned toCol(unsigned index) {
        return index / (HEIGHT+1);
    }

    inline static uint64 toMask(unsigned row, unsigned col) {
        return (uint64)1 << toIndex(row, col);
    }

    static uint64 parse(std::string board, char piece = 'X');
    static std::string print(uint64 board);

    static uint64 line4(uint64);
    static uint64 line3(uint64);
    static uint64 line2(uint64);

    inline static uint8 popcount(uint64 x) {
        uint8 c = 0;
        for (; x > 0; x &= x -1) c++;
        return c;
    }

    static uint64 flipLeftRight(uint64 x) {
        uint64 flip = uint64(0);
        for(unsigned col=0; col<WIDTH; col++) {
            for(unsigned row=0; row<HEIGHT; row++) {
                if(x & toMask(row, col)) {
                    flip |= toMask(row, WIDTH-col-1);
                }
            }
        }
        return flip;
    }

    static uint64 zeroBarrier; // Has a bit set for every valid piece location
    static uint64 baseBarrier; // Has a bit set in the bottom row
};

uint64 makeZeroBarrier();
uint64 makeBaseBarrier();


class GameState {

  public:
    GameState();
    GameState(uint64 p1, uint64 p2, Player player, Depth depth);

    GameState(const GameState& other);
    GameState& operator=(const GameState& other);

    bool operator==(const GameState& rhs) const;
    bool operator!=(const GameState& rhs) const;

    friend std::ostream& operator<<(std::ostream& os, const GameState& board);

    static GameState random(Depth depth=16);
    static GameState parse(std::string text);

    inline uint64 hash() const { return xorHash; }
    inline uint64 key() const {  return p1 | ((p1 | p2) + Bitboard::baseBarrier); }

    inline Player getPlayer() const { return player; }
    inline void setPlayer(Player p) { this->player = p; }
    inline bool isValid() const { return player != PLAYER_INVALID; }

    inline Depth getDepth() const { return depth; }
    inline void setDepth(Depth d) { this->depth = d; }

    void children(GameState* buffer) const;
    Value evaluate() const;
    uint8 heuristic() const;

    GameState flipLeftRight() const;

    std::string print() const;

    void assertInvariants() const;


  private:
    uint64 p1, p2;

    // Derived fields
    // These can be inferred from p1 and p2 but are kept explicit for efficiency
    uint64 xorHash;
    Player player;
    Depth depth;
    uint8 emptyPos[WIDTH];

    void generateDerivedFields();

    // Add a piece in the row and column
    void makeMove(unsigned row, unsigned col);

    // Internal hashing class
    // Partly to solve the problem of static initialisation of the
    // keys. Uses Zobrist hashing.
    struct Hasher {
        uint64 keys[(2*SIZE)+1];

        Hasher() {
            for(unsigned i=0; i<(2*SIZE)+1; i++) {
                keys[i] = (static_cast<uint64>(rand()) << 32);
                keys[i] |= static_cast<uint64>(rand());
            }
        }

        uint64 hash(uint64 p1_, uint64 p2_, int player_) const {
            uint64 h = 0;
            for(unsigned i=0; i<SIZE; i++) {
                if(p1_ & ((uint64)1 << i)) {
                    h ^= keys[i];
                }
                if(p2_ & ((uint64)1 << i)) {
                    h ^= keys[SIZE+i];
                }
            }
            if(player_ == PLAYER_MIN) {
                h ^= keys[2*SIZE];
            }
            return h;
        }

        uint64 incHash(uint64 hash, unsigned index, int player_) const {
            // XOR with the new move
            hash ^= keys[player_ == PLAYER_MAX ? index : index + SIZE];
            // Flip the player
            hash ^= keys[2*SIZE];
            return hash;
        }
    };

    static Hasher hasher;
};


#endif
