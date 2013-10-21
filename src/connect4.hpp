#ifndef CONNECT4
#define CONNECT4

#include <iostream>

#include "game.hpp"

const static int WIDTH = 7;
const static int HEIGHT = 6;
const static int SIZE = (HEIGHT+1) * WIDTH;

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

class Connect4 {

  public:
    Connect4();
    Connect4(uint64 p1, uint64 p2, int player, int depth);

    Connect4(const Connect4& other);
    Connect4& operator=(const Connect4& other);

//    Connect4(Connect4&& other);
//    Connect4& operator=(Connect4&& other);

    friend std::ostream& operator<<(std::ostream& os, const Connect4& board);

    bool operator==(const Connect4& rhs) const;
    bool operator!=(const Connect4& rhs) const;

    static Connect4 random(uint depth=16);

    inline uint64 hash() const { return xorHash; }
    inline uint64 key() const { return static_cast<uint32_t>(xorHash);
/*
        uint64 base = 0;
        for(unsigned col=0; col<WIDTH; col++) {
            base |= (uint64)1 << (col*(HEIGHT+1));
        }
        return p1 | (p1 + p2 + base);
*/
    }

    inline int getPlayer() const { return player; }
    inline void setPlayer(int p) { this->player = p; }

    inline int getDepth() const { return depth; }
    inline void setDepth(int d) { this->depth = d; }

    void children(Connect4* buffer) const;
    Value evaluate() const;
    Value heuristic() const;

    // Flip X's to O's
    void flipBoard();

    std::string print() const;
    void parse(std::string text, int player, int depth);

    bool isValid() { return player != PLAYER_INVALID; }

    void assertInvariants() const;

  private:
    uint64 p1, p2;

    int player;
    int depth;

    // Derived fields
    uint8_t emptyPos[WIDTH];
    uint64 xorHash;

    static uint64 zeroBarrier;

    uint64 line4(uint64) const;
    uint64 hotspot3(uint64, uint64) const;

    void generateDerivedFields();
    void makeMove(unsigned row, unsigned col);

    // Internal hashing class
    // Partly to solve the problem of static initialisation of the keys
    struct Hasher {
        uint64 keys[(2*SIZE)+1];

        Hasher() {
            Random* rand = new Random();
            for(int i=0; i<(2*SIZE)+1; i++) {
                keys[i] = rand->gen_uint64();
            }
            delete rand;
        }

        uint64 hash(uint64 p1_, uint64 p2_, int player_) const {
            uint64 h = 0;
            for(int i=0; i<SIZE; i++) {
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
