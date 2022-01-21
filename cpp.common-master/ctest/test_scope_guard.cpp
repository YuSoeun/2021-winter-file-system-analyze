#include "catch2/catch.hpp"

#include "scope_guard.hpp"

using namespace sg;

int count = 0;

TEST_CASE("Scope guard Test") {

  SECTION("with dismiss") {
    count = 0;
    {
      auto guard = make_scope_guard([=] { count = 1; });
      guard.dismiss();
      REQUIRE(count == 0);
    }

    REQUIRE(count == 0);
  }

  SECTION("without dismiss") {
    count = 0;
    {
      auto guard = make_scope_guard([=] { count = 1; });
      REQUIRE(count == 0);
    }

    REQUIRE(count == 1);
  }
}
