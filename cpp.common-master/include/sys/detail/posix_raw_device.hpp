#pragma once

#include <string>
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace sys::detail::posix {

  using namespace std;

  class raw_device
  {
  public:
    typedef int handle_type;

  public:
    raw_device() 
      : m_handle{-1}
    {}

    auto open(std::string const& disk) -> bool
    {
      return false;
    }

    auto is_open() const -> bool
    {
      return false;
    }

    auto close() -> void
    {
    }

    auto read(char* buffer, int count) -> int
    {
      return -1;
    }

    auto seek(int64_t offset, int dir) -> int64_t
    {
      return -1;
    }

    auto tell() -> int64_t
    {
      return -1;
    }

    auto size() const -> int64_t
    {
      return -1;
    }

    static auto size(string const& path) -> int64_t
    {
      return -1;
    }

    auto last_error() -> uint32_t
    {
      return 0;
    }

  private:
    handle_type m_handle;
  };
  
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
