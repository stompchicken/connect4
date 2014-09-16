#define CATCH_CONFIG_RUNNER
#include "unit_test/catch.hpp"

int main(int argc, char* const argv[]) {
    srand(1234);
    return Catch::Session().run(argc, argv);
}

