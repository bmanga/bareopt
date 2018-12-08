#include <catch2/catch.hpp>
#include "optional.h"

TEST_CASE("Observers", "[observers]") {
cm::optional<int> o1 = 42;
cm::optional<int> o2;
const cm::optional<int> o3 = 42;

REQUIRE(*o1 == 42);
REQUIRE(*o1 == o1.value());
REQUIRE(o2.value_or(42) == 42);
REQUIRE(o3.value() == 42);
auto success = std::is_same<decltype(o1.value()), int &>::value;
REQUIRE(success);
success = std::is_same<decltype(o3.value()), const int &>::value;
REQUIRE(success);
success = std::is_same<decltype(std::move(o1).value()), int &&>::value;
REQUIRE(success);
#ifndef CM_NO_CONSTREFREF
success = std::is_same<decltype(std::move(o3).value()), const int &&>::value;
REQUIRE(success);
#endif
}