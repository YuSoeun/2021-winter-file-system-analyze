#pragma once

#include "extent.hpp"
#include "byte_buffer2.hpp"

#include <boost/iostreams/categories.hpp>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace sys::io {

  using namespace std;

  class stream
  {
  public:
    stream(istream* ifs, extents const& extents, int64_t file_size=0, int64_t offset=0);

  public: // boost::istream interface
    typedef char char_type;
    typedef boost::iostreams::seekable_device_tag category;

    auto read(char* s, streamsize n) -> streamsize;
    auto to_bb(uint32_t from=0, uint32_t to=0) -> ByteBuffer2;

    auto write(char const* /*s*/, streamsize /*n*/) -> streamsize;
    auto seek(streampos off, ios_base::seekdir way=ios_base::beg) -> streampos;

  public:
    auto size() const { return m_actual_size; }
    auto extents_count() const { return m_extents.size(); }
    auto get_extents() const& { return m_extents; }
    auto add_front(extent const& e) -> bool;
    auto append(extent const& e) -> bool;
    auto append(extents const& e) -> bool;

  protected:
    extents m_extents;
    istream* m_stream{};

    int64_t m_offset{-1};
    int64_t m_actual_size{-1};
  };

}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
