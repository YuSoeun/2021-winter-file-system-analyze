#pragma once

#ifdef POSIX
#include "detail/posix_raw_device.hpp"
#define RAW_DEVICE_BACKEND sys::detail::posix::raw_device
#else
#include "detail/win32_raw_device.hpp"
#define RAW_DEVICE_BACKEND sys::detail::win32::raw_device
#endif

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace sys {

  template <typename RawDevice = RAW_DEVICE_BACKEND>
  class basic_raw_device
  {
  public:
    typedef RawDevice impl_type;

  public:
    basic_raw_device()
    {
    }

    auto open(std::string const& name) -> bool
    {
      return m_impl.open(name);
    }

    auto close() -> void
    {
      return m_impl.close();
    }

    auto read(char* buffer, int64_t length) -> int64_t
    {
      return m_impl.read(buffer, length);
    }

    auto seek(int64_t offset, int dir) -> int64_t
    {
      return m_impl.seek(offset, dir);
    }

    auto size() -> int64_t
    {
      return m_impl.size();
    }

    auto is_open() const -> bool
    {
      return m_impl.is_open();
    }

    auto tell() -> int64_t
    {
      return m_impl.tell();
    }

    static auto size(std::string const& path) -> int64_t
    {
      return RawDevice::size(path);
    }

    auto last_error() -> uint32_t
    {
      return m_impl.last_error();
    }

  private:
    impl_type m_impl;
  };

  using raw_device = basic_raw_device<>;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
