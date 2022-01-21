#include "catch2/catch.hpp"

#include "async_runnable.hpp"
#include <sstream>

#include <iostream>

using namespace std;

struct ResultCollector {
  int notify(int res) {
    sum += res;
    final_result = res;
    return res;
  }

  int sum{0};
  int final_result{0};
};

struct Carver2: public AsyncNotifiable<int, int> {
  void run() {
    for (int i=1; i<=100; i++) 
      last_called = notify_result(i);
  }

  int last_called{0};
};

TEST_CASE("Test async_runnable") {
  Carver2 carver;
  ResultCollector r;

  SECTION("asdf") {
    carver.result_notifier(
        [&r](auto p) { return r.notify(p); }
    );
    carver.run();

    REQUIRE(r.final_result == 100);
    REQUIRE(r.sum == 5050);
    REQUIRE(carver.last_called == 100);
  }
}
