#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main( int argc, char* const argv[] )
{
    srand(0);
    int result = Catch::Session().run(argc, argv);
    return result;
}

