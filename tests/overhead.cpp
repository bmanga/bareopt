#include <catch2/catch.hpp>
#include "optional.h"
#include <type_traits>

struct dummy
{
  int i;
  float f;
  double d;
};

TEST_CASE("Space Overhead", "[overhead]") {
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

TEST_CASE("Triviality", "[overhead]") {
  REQUIRE(std::is_trivially_destructible<cm::optional<dummy&>>::value ==
          std::is_trivially_destructible<dummy&>::value);
}
