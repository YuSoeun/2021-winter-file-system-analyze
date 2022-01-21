#include "catch2/catch.hpp"

#include "stream.hpp"
#include <boost/iostreams/stream.hpp>
#include <sstream>

using namespace std;
namespace bio = boost::iostreams;

TEST_CASE("Read node_stream via node_stream interface") {
  stringstream iss("0123456789");
  //                  --  -- -
  sys::io::extents extents;
  extents.push_back(sys::io::extent(2, 2));
  extents.push_back(sys::io::extent(6, 2));
  extents.push_back(sys::io::extent(9, 1));

  char buffer[10] = { 0 };
  sys::io::stream ns(&iss, extents);
  REQUIRE (ns.extents_count() == 3);

  SECTION("using node_stream interface") {
    ns.read(buffer, 5);
    string r(buffer, 5);

    REQUIRE (ns.size() == 5);
    REQUIRE (r == "23679");
  }

  SECTION("using boost::iostream interface") {
    bio::stream<sys::io::stream> in(ns);

    in.read(buffer, 5);
    string r(buffer, 5);

    REQUIRE (ns.size() == 5);
    REQUIRE (r == "23679");
  }
}

