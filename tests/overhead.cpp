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

TEST_CASE("Noexcept", "[overhead") {
  struct tmc_dummy
  {
    tmc_dummy() {};
    tmc_dummy(tmc_dummy &&) {};
  };

  struct ntmc_dummy
  {
    ntmc_dummy() noexcept = default;
    ntmc_dummy(tmc_dummy &&) noexcept {};
  };

  static_assert(!std::is_nothrow_move_constructible<tmc_dummy>::value, "");
  REQUIRE(std::is_nothrow_move_constructible<cm::optional<tmc_dummy>>::value ==
          std::is_nothrow_move_constructible<tmc_dummy>::value);

  static_assert(std::is_nothrow_move_constructible<tmc_dummy&>::value, "");
  REQUIRE(std::is_nothrow_move_constructible<cm::optional<tmc_dummy&>>::value ==
          std::is_nothrow_move_constructible<tmc_dummy&>::value);

  static_assert(std::is_nothrow_move_constructible<ntmc_dummy>::value, "");
  REQUIRE(std::is_nothrow_move_constructible<cm::optional<ntmc_dummy>>::value ==
          std::is_nothrow_move_constructible<ntmc_dummy>::value);

  REQUIRE(std::is_nothrow_move_constructible<cm::optional<ntmc_dummy&>>::value ==
          std::is_nothrow_move_constructible<ntmc_dummy&>::value);
}