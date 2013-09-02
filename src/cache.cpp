#include "cache.hpp"

#include <sstream>

bool operator==(const CacheValue& lhs, const CacheValue& rhs) {
    return lhs.lower == rhs.lower && lhs.upper == rhs.upper && lhs.move == rhs.move && lhs.depth == rhs.depth;
}

std::ostream& operator<<(std::ostream &output, const CacheValue &val) {
    output << "{" << val.lower << ", " << val.upper << ", ";
    output << static_cast<int>(val.move) << ", " << static_cast<int>(val.depth) << "}";
    return output;
}

uint64_t toEntry(Key key, CacheValue value) {
    uint64_t entry = static_cast<uint64_t>(key) << 32;
    entry |= static_cast<uint64_t>(value.lower) << 24;
    entry |= static_cast<uint64_t>(value.upper) << 16;
    entry |= static_cast<uint64_t>(value.move) << 8;
    entry |= static_cast<uint64_t>(value.depth);
    return entry;
}

Key toKey(uint64_t entry) {
    return static_cast<Key>(entry >> 32);
}

CacheValue toValue(uint64_t entry) {
    CacheValue value;
    value.lower = static_cast<uint8_t>(entry >> 24);
    value.upper = static_cast<uint8_t>(entry >> 16);
    value.move = static_cast<uint8_t>(entry >> 8);
    value.depth = static_cast<uint8_t>(entry);
    return value;
}

Cache::Cache(int bits) {
    this->mask = (static_cast<uint64_t>(1) << bits) - 1;

    this->capacity = static_cast<uint64_t>(1) << bits;
    this->size = 0;

    // Zeroes all entries
    this->hashtable = new uint64_t[capacity]();

    for(int i=0; i<DEPTH_MAX; i++) {
        counts[i] = 0;
    }

}

Cache::~Cache() {
    std::cout << "~Cache" << std::endl;
    delete [] this->hashtable;
}

bool Cache::get(const Connect4& board, CacheValue& value) const {
    uint64_t startIndex = board.hash() & this->mask;
    uint64_t index;
    uint64_t entry;

    for(uint64 offset = 0; offset < probe; offset++) {
        index = startIndex + offset;
        while(index >= this->capacity) {
            index -= this->capacity;
        }

        entry = this->hashtable[index];
        Key k = toKey(entry);
        CacheValue v = toValue(entry);
        if(k == 0) {
             return false;
        } else if(k == board.key()) {
            value = v;
//            std::cout << "GET: " << index << " " << k << " " << v << std::endl;
            return true;
        }
    }

    return false;
}

bool Cache::put(const Connect4& board, const CacheValue& value) {
    uint64_t startIndex = board.hash() & this->mask;
    uint64_t index;
    uint64_t entry;
    uint64_t newEntry = toEntry(board.key(), value);

    for(uint64 offset = 0; offset < probe; offset++) {
        index = startIndex + offset;
        while(index >= this->capacity) {
            index -= this->capacity;
        }

        entry = this->hashtable[index];
        Key k = toKey(entry);
        CacheValue v = toValue(entry);

        if(k == 0) {
            this->hashtable[index] = newEntry;
            this->counts[board.getDepth()] += 1;
            this->size += 1;
//            std::cout << "PUT: " << index << " " << board.key() << " " << value << std::endl;
            return true;
        } else if(k == board.key()) {
            this->hashtable[index] = newEntry;
            this->counts[board.getDepth()] += 1;
//            std::cout << "PUT: " << index << " " << board.key() << " " << value << std::endl;
            return true;
        } else if(v.depth > board.getDepth()) {
            this->hashtable[index] = newEntry;
            this->counts[v.depth] -= 1;
            this->counts[board.getDepth()] += 1;
//            std::cout << "PUT: " << index << " " << board.key() << " " << value << std::endl;
            return true;
        }
    }

    return false;
}

void Cache::resetBounds() {
    for(uint64 i=0; i<capacity; i++) {
        Key key = toKey(hashtable[i]);
        CacheValue value = toValue(hashtable[i]);
        value.lower = VALUE_MIN;
        value.upper = VALUE_MAX;
//        value.move = MOVE_INVALID;
        hashtable[i] = toEntry(key, value);
    }
}

std::string Cache::statistics() const {
    std::stringstream stats;
    stats.precision(4);

    for(int i=0; i<DEPTH_MAX; i++) {
        if(counts[i] > 0) {
            stats << "[" << i << "]\t" << this->counts[i] << std::endl;
        }
    }

    float load = float(this->size)/this->capacity;
    stats << "Load\t" << load << "\t(" << this->size << "/" << this->capacity << ")" << std::endl;

    return stats.str();
}
