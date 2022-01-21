#pragma once

#ifdef POSIX
#  include "net/detail/posix_socket.hpp"
#  define SOCKET_BACKEND net::detail::posix::socket
#elif UNDER_CE
#  include "net/detail/wince_socket.hpp"
#  define SOCKET_BACKEND net::detail::wince::socket
#else
#  include "net/detail/win32_socket.hpp"
#  define SOCKET_BACKEND net::detail::win32::socket
#endif

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace net {

  template <typename Socket = SOCKET_BACKEND>
  class basic_socket
  {
  public:
    typedef Socket socket_type;
    basic_socket()
    {}

    auto open(char const* ip, uint16_t port, uint32_t timeout) -> bool
    {
      return m_impl.open(ip, port, timeout);
    }

    auto close(bool hard_close=false) -> void
    {
      m_impl.close(hard_close);
    }

    auto read(byte_buffer& buffer, uint32_t to_read, uint32_t timeout=30*1000, bool hard_close=false) -> bool
    {
      return m_impl.recv(buffer, to_read, timeout, hard_close);
    }

    auto write(byte_buffer& buffer, uint32_t to_send, uint32_t& sent, uint32_t timeout=30*1000, bool hard_close=false) -> bool
    {
      return m_impl.send(buffer, to_send, sent, timeout, hard_close);
    }

    auto is_open() -> bool
    {
      return m_impl.is_open();
    }

    auto last_error() -> uint32_t 
    {
      return m_impl.last_error();
    }

  private:
    socket_type m_impl;
  };

  typedef basic_socket<> socket;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
