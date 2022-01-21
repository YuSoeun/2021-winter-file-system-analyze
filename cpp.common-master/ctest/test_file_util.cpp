#include "catch2/catch.hpp"

#include "file_util.hpp"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

TEST_CASE("File Util") {
  vector<uint8_t> arr = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
  string file_name = "test.bin";

  ofstream f;
  f.open(file_name, ios::binary);
  f.write((char*)&arr[0], 10);
  f.close();

  SECTION("exist and size") {
    REQUIRE(util::file::exists(file_name));
    REQUIRE(util::file::size(file_name) == 10);
  }

  SECTION("read smart pointer") {
    ifstream f;
    f.open(file_name, ios::binary);
    REQUIRE(f.is_open());

    auto res = util::file::read_sp(f, 0, 10);
    for (int i=0; i<10; i++) REQUIRE(res[i] == arr[i]);
  }

  remove(file_name.c_str());
}
