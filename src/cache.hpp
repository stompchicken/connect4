#ifndef CACHE
#define CACHE

#include "gameState.hpp"

/*
 Closed addressing hash table with 64-bit entries and linear probing
 Key/value pairs are packed into 64-bits
 Key/value replacement is done by replacing deeper states with
 shallower ones, on the assumption that they are more expensive to compute
 */

typedef uint32 Hash;
typedef uint64 Key;
typedef uint64 Packed;

struct Entry {
    // Lower bound on value of state
    Value lower;
    // Upper bound on value of state
    Value upper;
    // Depth of state
    Depth depth;
    // The index of the optimal move at this state
    uint8_t bestMove;
};

void packEntry(const Key& key, const Entry& entry, Packed& packed);
void unpackEntry(const Packed& value, Key& key, Entry& entry);
bool operator==(const Entry& lhs, const Entry& rhs);
std::ostream& operator<<(std::ostream &output, const Entry &val);

class Cache {
  public:
    Cache(uint64_t capacity);
    virtual ~Cache();
    void prefetch(const GameState& state) const;
    bool get(const GameState& state, Entry& value) const;
    bool put(const GameState& state, const Entry& value);
    void clear();
    // Only clean lower/upper bounds but keep best moves
    // This is used for something like iterative deepening
    void clearValues();
    std::string statistics() const;

  private:
    // Number of entries in the hashtable
    const uint64_t capacity;
    // Count of entries
    uint64_t size;
    // Contains key, value pairs
    uint64_t* hashtable;

    // Keep counts of how many states of each depth are in the cache
    mutable int counts[DEPTH_MAX];

    // Size of the linear probing before returning failure
    // (The size of a cache line in modern Intel chips is 64 bytes)
    const static uint64_t probe = 8;

    // No copying
    Cache(const Cache& other);
    Cache& operator=(const Cache& other);
};


#endif
