#ifndef CACHE
#define CACHE

#include <stdint.h>

#include "gameState.hpp"

typedef uint32_t Hash;
typedef uint64_t Key;
typedef uint64_t Packed;

struct Entry {
    Value lower;
    Value upper;
    Depth depth;
    uint8_t bestMove;
};

void packEntry(const Key& key, const Entry& entry, Packed& packed);
void unpackEntry(const Packed& value, Key& key, Entry& entry);

bool operator==(const Entry& lhs, const Entry& rhs);
std::ostream& operator<<(std::ostream &output, const Entry &val);

/*
 Closed addressing hash table with 64-bit entries and linear probing
 Replacements are made by depth
 */
class Cache {
  public:
    Cache(int bits);
    virtual ~Cache();
    bool get(const GameState& state, Entry& value) const;
    bool put(const GameState& state, const Entry& value);
    void clear();
    void clearValues();
    std::string statistics() const;

  private:
    // Bit mask on low-order bits to get index
    const uint64_t mask;
    // Number of entries in the hashtable
    const uint64_t capacity;
    // Count of entries
    uint64_t size;
    // Contains key, value pairs
    uint64_t* hashtable;

    // Debug stuff
    mutable int counts[DEPTH_MAX];

    // Size of the linear probing before returning failure
    // (The size of a cache line in modern Intel chips is 64 bytes)
    const static uint64_t probe = 8;

    Cache(const Cache& other);
    Cache& operator=(const Cache& other);
};


#endif
