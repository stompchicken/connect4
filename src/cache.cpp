#include "cache.hpp"
#include <sstream>
#include <bitset>
#include <cassert>

bool operator==(const Entry& lhs, const Entry& rhs) {
    return lhs.lower == rhs.lower && lhs.upper == rhs.upper &&
        lhs.depth == rhs.depth && lhs.bestMove == rhs.bestMove;
}

std::ostream& operator<<(std::ostream &output, const Entry &val) {
    output << "{ " << printValue(val.lower);
    output << ", " << printValue(val.upper);
    output << ", " << static_cast<int>(val.depth);
    output << ", " << static_cast<int>(val.bestMove) << " }";
    return output;
}


void packEntry(const Key& key, const Entry& entry, Packed& packed) {
#ifdef DEBUG
    assert(entry.lower < 4);
    assert(entry.upper < 4);
    assert(entry.lower <= entry.upper);
    assert(entry.depth < 64);
    assert(entry.bestMove < 8);
#endif

    packed = static_cast<uint64_t>(0);
    packed |= static_cast<uint64_t>(key) << 15;
    packed |= static_cast<uint64_t>(entry.lower) << 13;
    packed |= static_cast<uint64_t>(entry.upper) << 11;
    packed |= static_cast<uint64_t>(entry.depth) << 5;
    packed |= static_cast<uint64_t>(entry.bestMove) << 1;
}

void unpackEntry(const Packed& value, Key& key, Entry& entry) {
    key = static_cast<uint64_t>(value >> 15);
    entry.lower = static_cast<uint8_t>(value >> 13) & 0x3;
    entry.upper = static_cast<uint8_t>(value >> 11) & 0x3;
    entry.depth = static_cast<uint8_t>(value >> 5) & 0x3f;
    entry.bestMove = static_cast<uint8_t>(value >> 1) & 0x7;
}

Cache::Cache(int bits) : mask((static_cast<uint64_t>(1) << bits) - 1),
                         capacity(static_cast<uint64_t>(1) << bits) {
    size = 0;
    // Zero out the hashtable
    hashtable = new uint64_t[capacity]();
    for(int i=0; i<DEPTH_MAX; i++) {
        counts[i] = 0;
    }
}

Cache::~Cache() {
    delete [] this->hashtable;
}

bool Cache::get(const GameState& state, Entry& entry) const {
    uint64_t startIndex = state.hash() & this->mask;
    uint64_t index;
    Key key;

    for(uint64 offset = 0; offset < probe; offset++) {
        index = startIndex + offset;

        // Loop over the hashtable if we hit the end
        while(index >= this->capacity) {
            index -= this->capacity;
        }

        unpackEntry(this->hashtable[index], key, entry);
        if(key == 0) {
            return false;
        } else if(key == state.key()) {
            return true;
        } else {
            // Hash collision, advance
        }
    }

    return false;
}

bool Cache::put(const GameState& state, const Entry& entry) {

    int depth = (entry.depth < DEPTH_MAX) ? entry.depth : DEPTH_MAX - 1;
    Key key = state.key();

    uint64_t newValue;
    packEntry(key, entry, newValue);

    uint64_t startIndex = state.hash() & this->mask;
    uint64_t index;

    Key currentKey;
    Entry currentEntry;

    for(uint64 offset = 0; offset < probe; offset++) {
        index = startIndex + offset;

        // Loop over the hashtable if we hit the end
        while(index >= this->capacity) {
            index -= this->capacity;
        }

        unpackEntry(this->hashtable[index], currentKey, currentEntry);
        if(currentKey == 0) {
            // Insert new entry
            this->hashtable[index] = newValue;
            this->counts[depth] += 1;
            this->size += 1;
            return true;
        } else if(currentKey == key) {
            // Update entry
            this->hashtable[index] = newValue;
            this->counts[depth] += 1;
            return true;
        } else if(depth < currentEntry.depth) {
            // Replace entry
            this->hashtable[index] = newValue;
            this->counts[currentEntry.depth] -= 1;
            this->counts[depth] += 1;
            return true;
        } else {
            // Hash collision, advance
        }
    }

    return false;
}

void Cache::clear() {
    this->size = 0;
    for(uint64 i = 0; i < this->capacity; i++) {
        this->hashtable[i] = 0;
    }
    for(int i=0; i<DEPTH_MAX; i++) {
        counts[i] = 0;
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
