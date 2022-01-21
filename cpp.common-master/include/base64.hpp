#pragma once

#include <string>
#include <istream>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace util::codec::base64 {

  std::string decode(std::string_view encoded_string);
  std::string encode(std::string const& data);
  std::string encode(char const* bytes_to_encode, size_t in_len);

  void encode(std::istream& bytes_to_encode, std::string& ret);

}
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
