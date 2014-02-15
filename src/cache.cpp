#include "cache.hpp"
#include <sstream>

bool operator==(const Entry& lhs, const Entry& rhs) {
    return lhs.key == rhs.key && lhs.value == rhs.value && lhs.depth == rhs.depth;
}

std::ostream& operator<<(std::ostream &output, const Entry &val) {
    output << "{" << val.key << ", " << val.value << ", " << static_cast<int>(val.depth) << "}";
    return output;
}

uint64_t packEntry(Entry entry) {
    uint64_t value = static_cast<uint64_t>(0);
    value |= static_cast<uint64_t>(entry.key) << 15;
    value |= static_cast<uint64_t>(entry.value) << 7;
    value |= static_cast<uint64_t>(entry.depth);
    return value;
}

Entry unpackEntry(uint64_t value) {
    Entry entry;
    entry.key = static_cast<uint64_t>(value >> 15);
    entry.value = static_cast<uint8_t>(value >> 7);
    entry.depth = static_cast<uint8_t>(value) &~ 0x80; // We need to
                                                       // zero the 8th bit
    return entry;
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

    for(uint64 offset = 0; offset < probe; offset++) {
        index = startIndex + offset;

        // Loop over the hashtable if we hit the end
        while(index >= this->capacity) {
            index -= this->capacity;
        }

        entry = unpackEntry(this->hashtable[index]);
        if(entry.key == 0) {
//            std::cout << "GET index=" << index << " key=" << entry.key << " FAIL" << std::endl;
            return false;
        } else if(entry.key == state.key()) {
//            std::cout << "GET index=" << index << " key=" << entry.key << " SUCCEED" << std::endl;
            return true;
        } else {
//            std::cout << "GET index=" << index << " key=" << entry.key << " COLLISION" << std::endl;
        }
    }

    return false;
}

bool Cache::put(const GameState& state, const Entry& entry) {
    int depth = (entry.depth >= DEPTH_MAX) ? entry.depth : DEPTH_MAX - 1;
    uint64_t newValue = packEntry(entry);

    uint64_t startIndex = state.hash() & this->mask;
    uint64_t index;
    Entry currentEntry;

    for(uint64 offset = 0; offset < probe; offset++) {
        index = startIndex + offset;

        // Loop over the hashtable if we hit the end
        while(index >= this->capacity) {
            index -= this->capacity;
        }

        currentEntry = unpackEntry(this->hashtable[index]);

        if(currentEntry.key == 0) {
            // Insert new entry
//            std::cout << "PUT: index=" << index << " key=" << currentEntry.key << " NEW" << std::endl;
            this->hashtable[index] = newValue;
            this->counts[depth] += 1;
            this->size += 1;
            return true;
        } else if(currentEntry.key == state.key()) {
            // Update entry
//            std::cout << "PUT: index=" << index << " key=" << currentEntry.key << " UPDATE" << std::endl;
            this->hashtable[index] = newValue;
            this->counts[depth] += 1;
            return true;
        } else if(currentEntry.depth > depth) {
            // Replace entry
//            std::cout << "PUT: index=" << index << " key=" << currentEntry.key << " REPLACE" << std::endl;
            this->hashtable[index] = newValue;
            this->counts[currentEntry.depth] -= 1;
            this->counts[depth] += 1;
            return true;
        } else {
//            std::cout << "PUT: index=" << index << " key=" << currentEntry.key << " FAIL" << std::endl;
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
