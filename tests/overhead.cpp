#include <catch2/catch.hpp>
#include "optional.h"

TEST_CASE("Space Overhead", "[overhead]") {
  struct dummy
  {
    int i;
    float f;
    double d;
  };
  // no extra space for optional<T&> compared to T*
  REQUIRE(sizeof(cm::optional<dummy&>) == sizeof(dummy*));

  // requires 1 extra byte (plus eventual padding)
  REQUIRE(sizeof(cm::optional<bool>) == sizeof(bool) + sizeof(bool));

  struct dummy_optional {
    dummy payload;
    bool engaged;
  };
  REQUIRE(sizeof(cm::optional<dummy>) == sizeof(dummy_optional));
}
