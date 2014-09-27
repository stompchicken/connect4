#include "timer.hpp"

std::ostream& operator<<(std::ostream& os, const Timer& timer) {
    os << "n=" << timer.n << " mean=" << timer.mean << "s";
    return os;
}
