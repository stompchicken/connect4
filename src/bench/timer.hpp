#ifndef TIMER
#define TIMER

#include "common.hpp"

#include <ctime>
#include <iostream>
#include <limits>
#include <vector>
#include <time.h>

class Timer {

#define BILLION 1E9

public:
    Timer() : n(0) {
    }

    void start() {
        clock_gettime(CLOCK_MONOTONIC, &begin);
    }

    void stop() {
        clock_gettime(CLOCK_MONOTONIC, &end);
        int64 seconds = end.tv_sec - begin.tv_sec;
        int64 nanos = end.tv_nsec - begin.tv_nsec;
        update(seconds + nanos/BILLION);
    }

    double getQuantile(double q) const;

    friend std::ostream& operator<<(std::ostream& os, const Timer& timer);

private:
    struct timespec begin, end;

    long n;
    // I am a bad person for doing this...
    mutable std::vector<double> durations;
    
    void update(double duration);
};



#endif
