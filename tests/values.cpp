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