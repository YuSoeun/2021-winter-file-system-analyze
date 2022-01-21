#include "catch2/catch.hpp"

#include "progressable.hpp"
#include <sstream>

struct ProgressManager {
  void progress(int percent) {
    sum += percent;
    final_percent = percent;
  }

  int sum{0};
  int final_percent{0};
};

struct Carver: public Progressable {
  void run() {
    for (int i=1; i<=100; i++) notify_progress(i);
  }
};

TEST_CASE("Test progressable") {
  Carver carver;
  ProgressManager m;

  SECTION("accumulate all the progress") {
    carver.progress_notifier([&m](auto p) { m.progress(p); });
    carver.run();

    REQUIRE(m.final_percent == 100);
    REQUIRE(m.sum == 5050);
  }
}
