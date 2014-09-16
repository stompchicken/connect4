#ifndef GAMESTATE
#define GAMESTATE

#include <iostream>
#include <cstdlib>

#include "common.hpp"
#include "bitboard.hpp"


/*
 * Conect four board state
 */
class GameState {

  public:
    GameState();
    GameState(uint64 p1, uint64 p2, Player player, Depth depth);

    GameState(const GameState& other);
    GameState& operator=(const GameState& other);

    // Generate a random state by playing n random moves
    static GameState random(Depth depth, unsigned width=WIDTH, unsigned height=HEIGHT);
    static GameState parse(const std::string& text);

    // Incremetal Zobrist hash
    inline uint64 hash() const { return xorHash; }
    inline uint64 key() const {  return p1 | ((p1 | p2) + Bitboard::baseBarrier); }

    inline bool isValid() const { return player != PLAYER_INVALID; }

    inline Player getPlayer() const { return player; }
    inline void setPlayer(Player p) { this->player = p; }

    inline Depth getDepth() const { return depth; }
    inline void setDepth(Depth d) { this->depth = d; }

    // Generate all children nodes, takes a pre-allocated buffer
    void children(GameState* buffer, unsigned width=WIDTH, unsigned height=HEIGHT) const;
    // Test for win or draw conditions
    Value evaluate() const;
    // Give a rough estimate of board value
    Value heuristic() const;

    GameState flipLeftRight() const;

    std::string print() const;

    void assertInvariants() const;

    bool operator==(const GameState& rhs) const;
    bool operator!=(const GameState& rhs) const;

    friend std::ostream& operator<<(std::ostream& os, const GameState& board);


private:
    // Bitboards for player 1 (MAX) and player 2 (MIN)
    Bitboard::Board p1, p2;

    // Derived fields
    // These can be inferred from p1 and p2 but are kept explicit for efficiency
    uint64 xorHash;
    Player player;
    Depth depth;
    uint8 emptyPos[WIDTH];

    void generateDerivedFields();

    // Add a piece in the row and column and update derived fields
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
