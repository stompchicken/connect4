#ifndef CACHE
#define CACHE

#include <stdint.h>
#include <map>

#include "game.hpp"
#include "connect4.hpp"

// Size of the linear probing before returning failure
const static int probe = 8;

typedef uint32_t Key;

struct CacheValue {
    Value lower, upper;
    uint8_t move;
    uint8_t depth;
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
    bool get(const Connect4& board, CacheValue& value) const;
    bool put(const Connect4& board, const CacheValue& value);
    void resetBounds();
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
};


uint64_t toEntry(Key key, CacheValue value);
Key toKey(uint64_t entry);
CacheValue toValue(uint64_t entry);

#endif
