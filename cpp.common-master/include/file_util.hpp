#pragma once

#include <istream>
#include <fstream>
#include <cstdint>
#include <memory>
#include <string_view>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace util::file {

  using namespace std;

  bool exists(string_view path);

  auto size(string_view filename) -> int64_t;
  auto size(ifstream& in) -> int64_t;

  auto read_sp(istream& in, long offset, int size) -> unique_ptr<uint8_t[]>;
  auto read(istream& in, long offset, int size) -> uint8_t*;

  auto read_uint4_le(istream& in, long offset) -> uint32_t;
  auto read_uint4_be(istream& in, long offset) -> uint32_t;

}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
