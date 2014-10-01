#include "timer.hpp"

#include <algorithm>
#include <ios>
#include <iomanip>

void Timer::update(double duration) {
    n++;
    durations.push_back(duration);
}

double Timer::getQuantile(double q) const {
    size_t i = durations.size() * q;
    std::sort(durations.begin(), durations.end());
    return durations[i];
}

std::ostream& operator<<(std::ostream& os, const Timer& timer) {
    os << "n=" << timer.n; 

    os << std::scientific << std::setprecision(4);

    if(timer.n == 1) {
        os << " time=" << timer.durations[0] << "s";
    } else if(timer.n > 1) {
        os << " q05=" << timer.getQuantile(0.05) << "s";
        os << " q50=" << timer.getQuantile(0.50) << "s";
        os << " q95=" << timer.getQuantile(0.95) << "s";
    }

    return os;
}
