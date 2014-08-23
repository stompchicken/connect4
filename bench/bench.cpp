#include <iostream>

#include "timer.hpp"
#include "state.hpp"

int main();
void benchState();

int main() {

    std::cout << "Benchmarking state..." << std::endl;
    benchState();
}


void benchState() {

Timer timer;
for(int i=0; i<1000; i++) {
NaiveState state;
state.random(4);

timer.start();
state.evaluate();
timer.stop();
        }
        std::cout << "NaiveState.Evaluate" << std::endl;
        std::cout << timer << std::endl;
    }

    {
        Timer timer;
        for(int i=0; i<1000; i++) {
            BitboardState state;
            state.random(4);

            timer.start();
            state.evaluate();
            timer.stop();
        }
        std::cout << "BitobardState.Evaluate" << std::endl;
        std::cout << timer << std::endl;
    }

}
