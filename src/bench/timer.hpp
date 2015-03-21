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
    uint64 getSampleCount() const { return n; }

    friend std::ostream& operator<<(std::ostream& os, const Timer& timer);

private:
    struct timespec begin, end;

    uint64 n;
    // Required for getQuantile to be const
    // This feels pretty bad
    mutable std::vector<double> durations;
    
    void update(double duration);
};



#endif
