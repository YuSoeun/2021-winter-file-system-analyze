#include "sys/active_regex.hpp"
#include <boost/scoped_array.hpp>
#include <iostream>

using namespace std;
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////

namespace sys::text {
  
  ActiveRegex::ActiveRegex()
    : m_offset(0)
    , m_buffer_size(4096)
  {}

  bool ActiveRegex::search(boost::regex const& e, std::istream& is, bool active)
  {
    using namespace boost; 
    scoped_array<char> buf(new char[m_buffer_size]); char* const     pbuf = buf.get(); char const* next_pos = pbuf + m_buffer_size;

    matches results;
    int64_t match_count = 0;

    bool ok = true;
    while (ok)
    {
      // 0. determine whether or not we should go on
      if (should_stop())
        return false;

      // 1. compact partial matched string to buffer[0] if available
      //    invariant: left_over + next_pos == (buf + sizeof(buf))
      ptrdiff_t left_over = (pbuf + m_buffer_size) - next_pos;
      memmove(pbuf, next_pos, left_over);

      // 2. read text from the outer stream
      ptrdiff_t to_read = next_pos - pbuf;
      is.read(pbuf + left_over, to_read);

      // 3. update the execution condition
      std::streamsize read = is.gcount();
      ok = (read == to_read);

      // 4. increment offsets we read so far
      m_offset += read;

      // 5. reset next position
      next_pos = pbuf + m_buffer_size;

      // 6. search
      cregex_iterator cur(pbuf, pbuf+read+left_over, e, match_default|match_partial);
      cregex_iterator last;

      // 6. no match.
      //    prepare next position if no match occurred
      if (cur == last)
        continue;

      // 6. match found!
      //    discriminate partial vs. complete
      while (cur != last)
      {
        // 6.1) partial match
        if ((*cur)[0].matched == false)  
        {
          next_pos = (*cur)[0].first;
          break;
        }

        // 6.2) full match
        int64_t match_length = (*cur)[0].second - (*cur)[0].first;
        int64_t       offset = (*cur)[0].first  - pbuf;
        int64_t  base_offset = m_offset - read - left_over;
        uint32_t group_index = 0;

        if (cur->size() > 1)
        {
          for (size_t i=0; i<cur->size(); i++)
          {
            string const& s = (*cur)[i].str();
            if (!s.empty())
            {
              group_index = i;
              break;
            }
          }
        }

        if (active)
        {
          notify_result(match_result(base_offset + offset, match_length, group_index));
          match_count++;
        }
        else
        {
          m_results.push_back(match_result(base_offset + offset, match_length, group_index));
        }

        ++cur;
      }
    }

    return active ? match_count > 0 
                  : !m_results.empty();
  }

}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
