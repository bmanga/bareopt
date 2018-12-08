#include <catch2/catch.hpp>
#include "optional.h"
#include <vector>

struct foo {
  foo() = default;
  foo(foo&) = delete;
  foo(foo&&) {};
};

TEST_CASE("Constructors", "[constructors]") {
cm::optional<int> o1;
REQUIRE(!o1.has_value());

cm::optional<int> o2 = cm::nullopt;
REQUIRE(!o2);

cm::optional<int> o3 = 42;
REQUIRE(*o3 == 42);

cm::optional<int> o4 = o3;
REQUIRE(*o4 == 42);

cm::optional<int> o5 = o1;
REQUIRE(!o5);

cm::optional<int> o6 = std::move(o3);
REQUIRE(*o6 == 42);

cm::optional<short> o7 = 42;
REQUIRE(*o7 == 42);

cm::optional<int> o8 {o7};
REQUIRE(*o8 == 42);

cm::optional<int> o9 = std::move(o7);
REQUIRE(*o9 == 42);

{
cm::optional<int &> o;
REQUIRE(!o);

cm::optional<int &> oo = o;
REQUIRE(!oo);
}

{
auto i = 42;
cm::optional<int &> o = i;
REQUIRE(o);
REQUIRE(*o == 42);

cm::optional<int &> oo = o;
REQUIRE(oo);
REQUIRE(*oo == 42);
}

std::vector<foo> v;
v.emplace_back();
cm::optional<std::vector<foo>> ov(std::move(v));
REQUIRE((*ov).size() == 1);
}