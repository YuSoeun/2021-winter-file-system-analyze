#include "catch2/catch.hpp"
#include "sys/raw_device.hpp"

#include <string>

using namespace std;

TEST_CASE("RawDevice interface") {

  SECTION("create") {
    sys::raw_device raw;
    REQUIRE(raw.open("device") == false);
  }
}
