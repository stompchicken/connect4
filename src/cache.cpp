#include "cache.hpp"
#include <sstream>
#include <bitset>

bool operator==(const Entry& lhs, const Entry& rhs) {
    return lhs.value == rhs.value && lhs.depth == rhs.depth;
}

std::ostream& operator<<(std::ostream &output, const Entry &val) {
    output << "{" << val.value << ", " << static_cast<int>(val.depth) << "}";
    return output;
}


void packEntry(const Key& key, const Entry& entry, Packed& packed) {
    packed = static_cast<uint64_t>(0);
    packed |= static_cast<uint64_t>(key) << 15;
    packed |= static_cast<uint64_t>(entry.value) << 7;
    packed |= static_cast<uint64_t>(entry.depth);
}

void unpackEntry(const Packed& value, Key& key, Entry& entry) {
    key = static_cast<uint64_t>(value >> 15);
    entry.value = static_cast<uint8_t>(value >> 7);
    entry.depth = static_cast<uint8_t>(value) &~ 0x80; // We need to zero the 8th bit
}

Cache::Cache(int bits) : mask((static_cast<uint64_t>(1) << bits) - 1),
                         size(0),
                         capacity(static_cast<uint64_t>(1) << bits),
                         hashtable(new uint64_t[capacity]()) {
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
//        std::cout << "GET: index=" << index << " entry=" << entry;
        if(key == 0) {
//            std::cout << " FAIL" << std::endl;
            return false;
        } else if(key == state.key()) {
//            std::cout << " SUCCEED" << std::endl;
            return true;
        } else {
//            std::cout << " COLLISION" << std::endl;
        }
    }

    return false;
}

bool Cache::put(const GameState& state, const Entry& entry) {
    int depth = (entry.depth >= DEPTH_MAX) ? entry.depth : DEPTH_MAX - 1;
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

//        std::cout << "PUT: index=" << index << " " << currentEntry << " -> " << entry;
        if(currentKey == 0) {
            // Insert new entry
//            std::cout << " NEW" << std::endl;
            this->hashtable[index] = newValue;
            this->counts[depth] += 1;
            this->size += 1;
            return true;
        } else if(currentKey == key) {
            // Update entry
//            std::cout << " UPDATE" << std::endl;
            this->hashtable[index] = newValue;
            this->counts[depth] += 1;
            return true;
        } else if(currentKey > depth) {
            // Replace entry
//            std::cout << " REPLACE" << std::endl;
            this->hashtable[index] = newValue;
            this->counts[currentEntry.depth] -= 1;
            this->counts[depth] += 1;
            return true;
        } else {
            std::cout << " FAIL" << std::endl;
        }
    }

    return false;
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
