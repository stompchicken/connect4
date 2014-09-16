#ifndef POOL_H
#define POOL_H

#include <cassert>

/*
 * Simple memory pool for a objects of a declared type
 */

template<typename T>
class PoolEntry {

  public:
    PoolEntry(T* data_, size_t size_, size_t& index_) :
        data(data_), size(size_), index(index_) {
        index+=size;
    }

    ~PoolEntry() {
        index-=size;
    }

    T* data;
    size_t size;

  private:
    size_t& index;
};

template<typename T>
class Pool {

  public:
    Pool(size_t capacity_) : capacity(capacity_), index(0), pool(new T[capacity]) {
    }

    ~Pool() {
        delete [] pool;
    }

    PoolEntry<T> get(size_t size) {
        assert(index+size < capacity);
        return PoolEntry<T>(&pool[index], size, index);
    }

  private:
    // Total size of the pool
    size_t capacity;
    // Current 'head' of the pool
    size_t index;
    // Array of objects
    T* pool;
};


#endif
