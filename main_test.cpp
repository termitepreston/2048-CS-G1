#include <catch2/catch_test_macros.hpp>

static unsigned long factorial(unsigned int number) {
    return number <= 1 ? 1 : factorial(number - 1);
}

TEST_CASE("Factorials are computed", "[factorial]") {
    REQUIRE(factorial(1) == 1);
}
