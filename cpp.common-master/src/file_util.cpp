#include "file_util.hpp"

#include <fstream>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace util::file {

  using namespace std;

  bool exists(string_view path)
  {
    return ifstream{path.data()}.good();
  }

  auto size(string_view filename) -> int64_t
  {
    ifstream ifs(filename.data(), ios::ate | ios::binary);

    return ifs.tellg();
  }

  auto size(ifstream& ifs) -> int64_t
  {
    if (!ifs.good())
      return 0;

    auto backup_pos = ifs.tellg();
    ifs.seekg(0, ios::end);
    auto result = ifs.tellg();

    ifs.seekg(backup_pos, ios::beg);

    return result;
  }

  auto read(istream& in, long offset, int size) -> uint8_t*
  {
    uint8_t* buffer = nullptr;

    if (size > 0)
    {
      buffer = new uint8_t[size];
      in.seekg(offset);
      in.read((char*)buffer, size);
    }

    return buffer;
  }

  auto read_sp(istream& in, long offset, int size) -> unique_ptr<uint8_t[]>
  {
    unique_ptr<uint8_t[]> buffer(read(in, offset, size));

    return buffer;
  }

  auto read_uint4_le(istream& in, long offset) -> uint32_t
  {
    uint32_t result;

    in.seekg(offset);
    in.read(reinterpret_cast<char*>(&result), sizeof(uint32_t));

    return result;
  }

  template <typename T>
  auto endian_reverse(T value) -> T
  {
    auto it = reinterpret_cast<uint8_t*>(&value);
    reverse(it, it + sizeof(T));
    return *it;
  }

  auto read_uint4_be(istream& in, long offset) -> uint32_t
  {
    auto result = read_uint4_le(in, offset);

    return endian_reverse<uint32_t>(result);
  }

}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
