#ifndef POOL_H
#define POOL_H

template<typename T>
class Pool {

  public:
    Pool(size_t size_) : pool(new T[size_]), index(0), size(size_){

    }

    ~Pool() {
        delete [] pool;
    }

    T& get() {
        
        return pool[(index+1];
    }

  private:
    T* pool;
    int index;
    size_t size;

};


#endif
