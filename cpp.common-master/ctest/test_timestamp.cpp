#include "catch2/catch.hpp"

#include "sys/time_stamp.hpp"

TEST_CASE("Timestamp Interface") {
  SECTION("YearMonthDay") {
    int y = 2019;
    int m = 3;
    int d = 13;
    int h = 15;
    int M = 33;
    int s = 55;
    int us = 123456;
    sys::TimeStamp ts(y, m, d, h, M, s, us);

    REQUIRE(ts.to_s().starts_with("2019-03-13 15:33:55.123456"));
  }

  SECTION("iso6501") {
    sys::TimeStamp ts(time_t(1552287917));
    REQUIRE(ts.to_s().starts_with("2019-03-11 07:05:17.000000"));
  }
}
