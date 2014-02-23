#ifndef CACHE
#define CACHE

#include <stdint.h>

#include "gameState.hpp"

typedef uint32_t Hash;
typedef uint64_t Key;

struct Entry {
    Key key;
    Value value;
    Depth depth;
};

uint64_t packEntry(Entry entry);
Entry unpackEntry(uint64_t value);

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
    std::string statistics() const;

  private:
    // Bit mask on low-order bits to get index
    uint64_t mask;
    // Count of entries
    uint64_t size;
    // Number of entries in the hashtable
    uint64_t capacity;
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
