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
    assert(entry.bestMove < WIDTH || entry.bestMove == MOVE_INVALID);
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

#ifdef DEBUG
    assert(entry.lower < 4);
    assert(entry.upper < 4);
    assert(entry.lower <= entry.upper);
    assert(entry.depth < 64);
    assert(entry.bestMove < WIDTH || entry.bestMove == MOVE_INVALID);
#endif
}

Cache::Cache(uint64 capacity_, uint64 probe_, bool replace) : 
    capacity(capacity_), probe(probe_), replacement(replace), size(0),
    hashtable(new uint64_t[capacity]) {

    for(uint64 i=0; i<capacity; i++) {
        this->hashtable[i] = 0;
    }

    for(int i=0; i<DEPTH_MAX; i++) {
        counts[i] = 0;
    }
}

Cache::~Cache() {
    delete [] this->hashtable;
}

void Cache::prefetch(const GameState& state) const {
    uint64_t index = state.hash() % this->capacity;
    __builtin_prefetch(&hashtable[index]);
}

bool Cache::get(const GameState& state, Entry& entry) const {
    uint64_t startIndex = state.hash() % this->capacity;
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

    uint64_t startIndex = state.hash() % this->capacity;
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
        } else if(replacement && depth < currentEntry.depth) {
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

void Cache::clearValues() {
    Key key;
    Entry entry;
    for(uint64 index=0; index<this->capacity; index++) {
        unpackEntry(this->hashtable[index], key, entry);
        entry.lower = VALUE_MIN;
        entry.upper = VALUE_MAX;
        packEntry(key, entry, this->hashtable[index]);
    }
}

std::string Cache::statistics() const {
    std::stringstream stats;
    stats.precision(4);

    float load = float(this->size*100)/this->capacity;
    stats << "Load:\t\t" << load << "%%" << std::endl;

    int maxDepth = 0;
    for(int i=0; i<DEPTH_MAX; i++) {
        if(counts[i] > 0) {
            maxDepth = i;
        }
    }

    for(int i=0; i<=maxDepth; i++) {
        stats << "[" << i << "]\t" << this->counts[i] << std::endl;
    }

    return stats.str();
}
