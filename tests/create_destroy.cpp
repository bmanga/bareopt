#include <catch2/catch.hpp>
#include "optional.h"

int counter = 0;
int ccounter = 0;
int mcounter = 0;
int dcounter = 0;


struct silly_matcher
{
  silly_matcher() { ++counter; }
  silly_matcher(const silly_matcher&) { ++ccounter; }
  silly_matcher(silly_matcher &&) { ++mcounter; }
  silly_matcher& operator =(const silly_matcher&) = default;
  ~silly_matcher() { ++dcounter; }
};

TEST_CASE("Matching Ctor-Dtor", "[matching-cdtor]") {
  {
    cm::optional<silly_matcher> om;
    silly_matcher sm; // default ctor
    om = sm; // copy ctor
    cm::optional<silly_matcher> om2 = om; // copy ctor
    cm::optional<silly_matcher> om3{om}; // copy ctor
    cm::optional<silly_matcher> om4 {std::move(sm)}; //move ctor
    cm::optional<silly_matcher> om5 = std::move(om2); // move ctor

    REQUIRE(counter  == 1);
    REQUIRE(mcounter == 2);
    REQUIRE(ccounter == 3);
  }
  REQUIRE(dcounter - counter - ccounter - mcounter == 0);
}
