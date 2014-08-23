#ifndef TIMER
#define TIMER

#include <ctime>
#include <iostream>

class Timer {

public:
    Timer() {
        n = 0;
        mean = 0.0;
    }

    void start() {
        time = std::clock();
    }

    void stop() {
        update((std::clock() - time) / (double) CLOCKS_PER_SEC);
    }

    double getMean() { return mean; }

    friend std::ostream& operator<<(std::ostream& os, const Timer& timer);

private:
    std::clock_t time;
    long n;
    double mean;

    void update(double duration) {
        n++;
        mean += (duration - mean) / n;
    }

};

std::ostream& operator<<(std::ostream& os, const Timer& timer) {
    os << "n=" << timer.n << " mean=" << timer.mean << "s";
    return os;
}


#endif
