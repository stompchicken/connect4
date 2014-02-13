#ifndef CACHE
#define CACHE

#include <stdint.h>

#include "gameState.hpp"

// Size of the linear probing before returning failure
// The size of a cache line in most Intel chips is 64 bytes
const static uint64_t probe = 8;

typedef uint32_t Key;

struct CacheValue {
    Value value;
    uint8_t depth;
    uint16_t padding;
};

bool operator==(const CacheValue& lhs, const CacheValue& rhs);
std::ostream& operator<<(std::ostream &output, const CacheValue &val);

/*
 Closed addressing hash table with 64-bit entries and linear probing
 Replacements are made by depth
 */
class Cache {
  public:
    Cache(int bits);
    virtual ~Cache();
    bool get(const GameState& state, CacheValue& value) const;
    bool put(const GameState& state, const CacheValue& value);
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

    Cache(const Cache& other);
    Cache& operator=(const Cache& other);
};

uint64_t toEntry(Key key, CacheValue value);
Key toKey(uint64_t entry);
CacheValue toValue(uint64_t entry);

#endif
