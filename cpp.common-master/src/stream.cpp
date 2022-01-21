#include "stream.hpp"

#include <iostream>
#include <stdexcept>
#include <numeric>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace sys::io::detail {
  
  struct offset_count
  {
    offset_count(): offset(0), count(0) {}
    offset_count(int o, int64_t c) :offset(o), count(c) {}
    
    int offset;
    int64_t count;
  };
  
  auto convert_offset(extents const& extents, int64_t offset) -> offset_count 
  {
    offset_count result;
    
    auto sum = 0L;
    for (int i=0; i<extents.size(); ++i)
    {
      auto e = extents[i];
      sum += e.count();
      if (sum >= offset)
        return offset_count(i, e.count() - sum + offset);
    }
    
    return result;
  }

}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace sys::io {

  stream::stream(istream* ifs, extents const& extents, int64_t file_size, int64_t offset)
    : m_stream(ifs)
  {
    m_extents = extents;
    m_offset  = offset;
    
    m_actual_size = file_size > 0 
      ? file_size 
      : accumulate(m_extents.begin(), m_extents.end(), 0L, 
                   [](int64_t sum, extent const& e) { return sum + e.count(); });
  }
  
  // Note
  // below 3 member functions are really important to be boost::iostreams
  auto stream::read(char* s, streamsize n) -> streamsize
  {
    if (n == 0 or m_offset >= m_actual_size)
      return 0;
    
    if (n > m_actual_size - m_offset)
      n = m_actual_size - m_offset;
    
    auto r0 = detail::convert_offset(m_extents, m_offset);
    auto r1 = detail::convert_offset(m_extents, m_offset + n);
    auto se = r0.offset; auto ee = r1.offset;
    auto se_offset = r0.count; auto ee_offset = r1.count;
      
    if (se == -1 or ee == -1)
      return 0;
    
    streamsize total_read = 0;
    
    int offset = 0;
    for (auto i = se; i <= ee; i++)
    {
      auto& extent = m_extents[i];
      auto to_read = (i == ee)
          ? ee_offset - se_offset
          : extent.count() - se_offset;
      
      if (to_read > 0)
      {
        m_stream->seekg(extent.offset() + se_offset);
        m_stream->read (s + offset, to_read);
        auto rd = m_stream->gcount();
        if (rd < 0)
          break;

        total_read += rd;
        offset += rd;
      }
      
      se_offset = 0;
    }
    
    m_offset += total_read;
    
    return total_read;
  }
  
  auto stream::to_bb(uint32_t from, uint32_t sz) -> ByteBuffer2
  {
    auto buffer = new uint8_t[sz];
    auto org_pos = m_stream->tellg();
    
    this->seek(from);
    this->read((char *)buffer, sz);
    this->seek(org_pos);
    
    return ByteBuffer2(buffer, 0, sz, true);
  }
  
  auto stream::write(char const* /*s*/, streamsize /*n*/) -> streamsize
  {
    throw logic_error("should not be called");

    return 0;
  }
  
  auto stream::seek(streampos offset, ios_base::seekdir dir) -> streampos
  {
    if (dir == ios_base::beg)
    {
      m_offset = offset;
    }
    else if (dir == ios_base::cur)
    {
      m_offset += offset;
    }
    else if (dir == ios_base::end)
    {
      if (m_offset - offset < 0)
        throw out_of_range("node_stream::seek: out of range");
      
      m_offset -= offset;
    }
    
    return m_offset;
  }

  auto stream::add_front(extent const& e) -> bool
  {
    m_extents.push_front(e);

    return true;
  }

  auto stream::append(extent const& e) -> bool
  {
    m_extents.push_back(e);

    return true;
  }

  auto stream::append(extents const& es) -> bool
  {
    for (auto& e: es) m_extents.push_back(e);

    return true;
  }

}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
