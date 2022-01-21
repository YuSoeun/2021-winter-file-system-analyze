#include "base64.hpp"

#include <functional>
#include <stdint.h>

#if defined(WIN32) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace util::codec::base64 {

  static const string base64_chars 
      = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

  inline bool is_base64(unsigned char c) 
  {
    return (isalnum(c) || (c == '+') || (c == '/'));
  }

  inline void encode(function<bool()> is_stop, 
      function<void(unsigned char&)> get_next, string& ret)
  {
    int i = 0;
    int j = 0;
    unsigned char ch_arr3[3];
    unsigned char ch_arr4[4];
    size_t prev = 0;

    while (!is_stop())
    {
      get_next(ch_arr3[i++]);
      if (i == 3)
      {
        ch_arr4[0] = (ch_arr3[0] & 0xfc) >> 2;
        ch_arr4[1] = ((ch_arr3[0] & 0x03) << 4) + ((ch_arr3[1] & 0xf0) >> 4);
        ch_arr4[2] = ((ch_arr3[1] & 0x0f) << 2) + ((ch_arr3[2] & 0xc0) >> 6); ch_arr4[3] = ch_arr3[2] & 0x3f;

        for(i = 0; (i <4) ; i++)
          ret += base64_chars[ch_arr4[i]];
        if (ret.length() - prev >= 76) { ret += "\n"; prev = ret.length(); }
        i = 0;
      }
    }

    if (i)
    {
      for (j = i; j < 3; j++)
        ch_arr3[j] = '\0';

      ch_arr4[0] = (ch_arr3[0] & 0xfc) >> 2;
      ch_arr4[1] = ((ch_arr3[0] & 0x03) << 4) + ((ch_arr3[1] & 0xf0) >> 4);
      ch_arr4[2] = ((ch_arr3[1] & 0x0f) << 2) + ((ch_arr3[2] & 0xc0) >> 6);
      ch_arr4[3] = ch_arr3[2] & 0x3f;

      for (j = 0; (j < i + 1); j++)
        ret += base64_chars[ch_arr4[j]];

      while ((i++ < 3))
        ret += '=';
    }
  }

  void encode(istream& bytes_to_encode, string& ret)
  {
    encode([&]() -> bool
    {
        return bytes_to_encode.eof();
    },
        [&](unsigned char& buf)
    {
        bytes_to_encode.get(*((char*)&buf));
    }, ret);
  }

  string encode(char const* bytes_to_encode, size_t in_len)
  {
    string ret;

    encode([&]() -> bool { return (in_len-- == 0); },
        [&](unsigned char& buf) { buf = *(bytes_to_encode++); }, 
        ret);

    return ret;
  }

  string encode(string const& data) 
  {
    return encode((char const*)data.data(), data.length());
  }

  string decode(string_view encoded_string)
  {
    int in_len = (int)encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char ch_arr4[4], ch_arr3[3];
    string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
      ch_arr4[i++] = encoded_string[in_]; in_++;
      if (i ==4) 
      {
        for (i = 0; i <4; i++)
          ch_arr4[i] = (unsigned char)base64_chars.find(ch_arr4[i]);

        ch_arr3[0] = (ch_arr4[0] << 2) + ((ch_arr4[1] & 0x30) >> 4);
        ch_arr3[1] = ((ch_arr4[1] & 0xf) << 4) + ((ch_arr4[2] & 0x3c) >> 2);
        ch_arr3[2] = ((ch_arr4[2] & 0x3) << 6) + ch_arr4[3];

        for (i = 0; (i < 3); i++)
          ret += ch_arr3[i];
        i = 0;
      }
    }

    if (i)
    {
      for (j = i; j <4; j++) ch_arr4[j] = 0;

      for (j = 0; j <4; j++)
        ch_arr4[j] = (unsigned char)base64_chars.find(ch_arr4[j]);

      ch_arr3[0] = (ch_arr4[0] << 2) + ((ch_arr4[1] & 0x30) >> 4);
      ch_arr3[1] = ((ch_arr4[1] & 0xf) << 4) + ((ch_arr4[2] & 0x3c) >> 2);
      ch_arr3[2] = ((ch_arr4[2] & 0x3) << 6) + ch_arr4[3];

      for (j = 0; (j < i - 1); j++) ret += ch_arr3[j];
    }

    return ret;
  }
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
