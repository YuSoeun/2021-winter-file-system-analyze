#include "catch2/catch.hpp"

#include "sqlite3/connection.hpp"
#include "sqlite3/statement.hpp"
#include <string>
#include <cstdio>

using namespace std;
using namespace api::sqlite3;

auto db_name = "aaa.db";

TEST_CASE("app::sqlite3 interface") {

  remove(db_name);

  SECTION("Connection") {
    connection c;
    c.open(db_name);
    c.execute("create table hens(id int primary key, name text)");

    statement s;
    s.prepare(c, "insert into hens(id, name) values(?, ?)");

    s.bind(1, 101);
    s.bind(2, "henrietta");
    s.step();

    s.reset();

    s.bind(1, 102);
    s.bind(2, "duck");
    s.step();

    REQUIRE(c.rowid() == 2);

    s.prepare(c, "select * from hens");
    s.step();
    REQUIRE(s.get_int(0) == 101);
    REQUIRE(s.get_string(1) == "henrietta");

    s.step();
    REQUIRE(s.get_int(0) == 102);
    REQUIRE(s.get_string(1) == "duck");
  }

  remove(db_name);
}
