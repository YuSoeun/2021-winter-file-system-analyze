#pragma once

#include <cstdint>
#include <deque>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace sys::io {

  class extent
  {
  public:
    extent(): extent(-1L, -1L) {}

    extent(int64_t offset, int64_t count): m_offset{offset}, m_count{count}
    {
    }

    auto contains(int64_t offset) const -> bool
    {
      return m_offset >= 0 &&
             offset >= m_offset && offset < m_offset + m_count;
    }

    auto empty() const -> bool
    {
      return m_offset == -1L && m_count == -1L;
    }

    auto offset() const { return m_offset; }
    auto count()  const { return m_count;  }

    friend bool operator<(extent const& lhs, extent const& rhs)
    {
      return lhs.m_offset < rhs.m_offset;
    }

    auto unit() -> char const* { return "byte"; }

  private:
    int64_t m_offset;
    int64_t m_count;
  };

  using extents = std::deque<extent>;

}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
