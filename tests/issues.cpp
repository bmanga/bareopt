#include <catch2/catch.hpp>
#include "optional.h"
#include <type_traits>

struct foo{
  int& v() { return i; }
  int i = 0;
};

int& x(int& i) { i = 42; return i;}


struct fail_on_copy_self {
  int value;
  fail_on_copy_self(int v) : value(v) {}
  fail_on_copy_self(const fail_on_copy_self& other) : value(other.value) {
    REQUIRE(&other != this);
  }
};

TEST_CASE("issue 15") {
cm::optional<fail_on_copy_self> o = fail_on_copy_self(42);

o = o;
REQUIRE((*o).value == 42);
}
