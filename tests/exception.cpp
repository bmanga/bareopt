#include <catch2/catch.hpp>
#include "optional.h"

TEST_CASE("Throw", "[exceptions]") {
  int caught = 0;
  cm::optional<int> oi;
  try {
    ++caught;
    *oi = 3;
    caught += 10;
  }
  catch(const cm::bad_optional_access &) {
    --caught;
  }
  REQUIRE(caught == 0);

  oi = 1;
  try {
    caught += 10;
    *oi = 5;
    caught -= 10;
  }
  catch(const cm::bad_optional_access&) {
    --caught;
  }
  REQUIRE(caught == 0);

  oi = cm::nullopt;
  try {
    ++caught;
    oi.value() = 5;
    caught -= 10;
  }
  catch(const cm::bad_optional_access&) {
    --caught;
  }
  REQUIRE(caught == 0);
}