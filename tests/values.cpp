#include <catch2/catch.hpp>
#include "optional.h"
#include <type_traits>

template <class A, class B>
bool checker()
{
  return std::is_same<A, B>::value;
}

TEST_CASE("LValues", "[values]") {
  std::string s = "hello";
  const cm::optional<std::string&> os = s;
  REQUIRE(checker<const std::string&, decltype(os.value())>());
  REQUIRE(checker<const std::string&, decltype(os.operator*())>());

  cm::optional<std::string&> os2 = s;
  REQUIRE(checker<std::string&, decltype(os2.value())>());
  REQUIRE(checker<std::string&, decltype(os2.operator*())>());


  const cm::optional<std::string> osc = s;
  REQUIRE(checker<const std::string&, decltype(osc.value())>());
  REQUIRE(checker<const std::string&, decltype(osc.operator*())>());

  cm::optional<std::string> os2c = s;
  REQUIRE(checker<std::string&, decltype(os2c.value())>());
  REQUIRE(checker<std::string&, decltype(os2c.operator*())>());
}


TEST_CASE("RValues", "[values]") {
  std::string s = "hello";

#ifndef CM_NO_CONSTREFREF
  const cm::optional<std::string&> os = s;
  REQUIRE(checker<const std::string&, decltype(std::move(os).value())>());
  REQUIRE(checker<const std::string&, decltype(std::move(os).operator*())>());

  const cm::optional<std::string> osc = s;
  REQUIRE(checker<const std::string&&, decltype(std::move(osc).value())>());
  REQUIRE(checker<const std::string&&, decltype(std::move(osc).operator*())>());
#endif

  cm::optional<std::string&> os2 = s;
  REQUIRE(checker<std::string&, decltype(std::move(os2).value())>());
  REQUIRE(checker<std::string&, decltype(std::move(os2).operator*())>());


  cm::optional<std::string> os2c = s;
  REQUIRE(checker<std::string&&, decltype(std::move(os2c).value())>());
  REQUIRE(checker<std::string&&, decltype(std::move(os2c).operator*())>());
}

TEST_CASE("ValueOr", "[values]") {
  std::string s = "hello world";
  cm::optional<std::string> os = s;
  cm::optional<std::string> ose;
  REQUIRE(checker<std::string, decltype(os.value_or("hello"))>());
  REQUIRE(checker<std::string, decltype(ose.value_or("hello"))>());
  REQUIRE(checker<std::string, decltype(std::move(os).value_or("hello"))>());
  REQUIRE(checker<std::string, decltype(std::move(ose).value_or("hello"))>());

  cm::optional<std::string&> osr = s;
  cm::optional<std::string&> osre;
  REQUIRE(checker<std::string &, decltype(osr.value_or(s))>());
  REQUIRE(checker<std::string &, decltype(osre.value_or(s))>());
  REQUIRE(checker<std::string &, decltype(std::move(osr).value_or(s))>());
  REQUIRE(checker<std::string &, decltype(std::move(osre).value_or(s))>());
}

TEST_CASE("MoveOnly", "[values]") {
  struct move_only
  {
    move_only() = default;
    move_only(const move_only &) { copied = true; };
    move_only(move_only &&) = default;
    move_only &operator =(const move_only &) { copied = true; };
    move_only &operator =(move_only &&) = default;

    bool copied = false;
  };

  cm::optional<move_only> omo;
  move_only mo;
  omo = std::move(mo);
  move_only mo2 = std::move(omo).value();
  move_only mo3 = std::move(omo).value_or(move_only{});
  REQUIRE(!mo.copied);
  REQUIRE(!mo2.copied);
  REQUIRE(!mo3.copied);
}

TEST_CASE("emplace", "[values]") {
  struct foo {
    foo(int x, double y) : x(x), y(y) {}
    int x;
    double y;
  };
  cm::optional<foo> o;
  o.emplace(2, 4.4);

  REQUIRE(o.has_value());
  REQUIRE((*o).x == 2);
  REQUIRE((*o).y == 4.4);

  cm::optional<foo &> ofr;
  foo f{8, -8.8};
  ofr.emplace(f);
  REQUIRE(ofr.has_value());
  REQUIRE((*ofr).x == 8);
  f.y = -1.1;
  REQUIRE((*ofr).y == -1.1);
}