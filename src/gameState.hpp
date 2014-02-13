#ifndef GAMESTATE
#define GAMESTATE

#include <iostream>
#include <cstdlib>
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

/*
const static unsigned WIDTH = 7;
const static unsigned HEIGHT = 6;
const static unsigned SIZE = (HEIGHT+1) * WIDTH;
*/

#define WIDTH 7
#define HEIGHT 6
#define SIZE 49



/*

Bitboard structure, column major with zero barrier at max height

X = P1
O = P2

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

    inline static unsigned popcount(uint64 x) {
        int c = 0;
        for (; x > 0; x &= x -1) c++;
        return c;
    }

    static uint64 zeroBarrier;
};

class GameState {

  public:
    GameState();
    GameState(uint64 p1, uint64 p2, int player, int depth);

    GameState(const GameState& other);
    GameState& operator=(const GameState& other);

//    GameState(GameState&& other);
//    GameState& operator=(GameState&& other);

    friend std::ostream& operator<<(std::ostream& os, const GameState& board);

    bool operator==(const GameState& rhs) const;
    bool operator!=(const GameState& rhs) const;

    static GameState random(Depth depth=16);

    inline uint64 hash() const { return xorHash; }
    inline uint64 key() const {
        return static_cast<uint32_t>(xorHash);
/*
        uint64 base = 0;
        for(unsigned col=0; col<WIDTH; col++) {
            base |= (uint64)1 << (col*(HEIGHT+1));
        }
        return p1 | (p1 + p2 + base);
*/
    }

    inline int getPlayer() const { return player; }
    inline void setPlayer(Player p) { this->player = p; }
    inline bool isValid() const { return player != PLAYER_INVALID; }

    inline int getDepth() const { return depth; }
    inline void setDepth(Depth d) { this->depth = d; }

    void children(GameState* buffer) const;
    Value evaluate() const;
    Value heuristic() const;

    std::string print() const;
    void parse(std::string text, int player, int depth);

    GameState flip() const;

    void assertInvariants() const;


  private:
    uint64 p1, p2;

    Player player;
    int depth;

    // Derived fields
    uint8_t emptyPos[WIDTH];
    uint64 xorHash;

    void generateDerivedFields();
    void makeMove(unsigned row, unsigned col);

    // Internal hashing class
    // Partly to solve the problem of static initialisation of the keys
    struct Hasher {
        uint64 keys[(2*SIZE)+1];

        Hasher() {
            for(unsigned i=0; i<(2*SIZE)+1; i++) {
                keys[i] = (static_cast<uint64>(rand()) << 32) | rand();
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
            hash ^= keys[player_ == PLAYER_MAX ? index : index + SIZE];
            hash ^= keys[2*SIZE];
            return hash;
        }
    };

    static Hasher hasher;
};

#endif
