#pragma once

#include "byte_buffer.h"
#include "exception.h"

#include <strings.h>    // for bzero
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace net::detail::posix {

  class socket
  {
  public:
    typedef int socket_type;

  public:
    socket(): m_socket(-1)
    {
      m_is_open = false;
      m_last_error = PCL_INTERNAL_ERROR;
    }

    ~socket()
    {
      close();
    }

    auto open(char const* ip, uint16_t port, uint32_t timeout) -> bool
    {
      sockaddr_in serv;
      bzero((void*)&serv, sizeof(serv));
      serv.sin_family = AF_INET;
      serv.sin_addr.s_addr = ::inet_addr(ip);
      serv.sin_port = ::htons(port);

      m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (m_socket < 0)
      {
        return false;
      }

      if (::connect(m_socket, (sockaddr*)&serv, sizeof(serv)) < 0)
      {
        return false;
      }

      m_is_open = true;

      return true;
    }

    auto close(bool hard_close=false) -> bool
    {
      m_is_open = false;

      if (m_socket == -1)
        return;

      if (hard_close)
      {
        linger l = { 1, 0 };
        ::setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(linger));
      }

      ::close(m_socket);
      m_socket = -1;
    }

    auto recv(byte_buffer& buffer, uint32_t bytes, uint32_t timeout=30*1000, bool hard_close=false) -> bool
    {
      int const BLOCK_SIZE = 1024;

      int read_bytes = 0;
      int left_bytes = bytes;
      uint8_t local_buffer[BLOCK_SIZE] = { 0 };
      do
      {
        int to_read = (left_bytes > BLOCK_SIZE) ? BLOCK_SIZE : left_bytes;
        int recved = ::recv(m_socket, (char*)local_buffer, to_read, 0);
        if (recved == 0) 
        {
          m_last_error = PCL_CONNECTION_CLOSED;
          return false;
        }
        
        if (recved < 0) 
        {
          m_last_error = PCL_RECV_ERROR;
          return false;
        }
        left_bytes -= recved;
        read_bytes += recved;
        buffer.set_stream(local_buffer, recved);
      }
      while(left_bytes > 0);

      return (read_bytes == bytes);
    }

    auto send(uint8_t* buffer, uint32_t bytes, uint32_t& sent, uint32_t timeout=30*1000, bool hard_close=false) -> bool
    {
      int const BLOCK_SIZE = 1024;

      int sent_bytes = 0;
      int left_bytes = bytes;
      do
      {
        int to_write = (left_bytes > BLOCK_SIZE) ? BLOCK_SIZE : left_bytes;
        int sent_    = ::send(m_socket, (char*)(buffer+sent_bytes), to_write, 0);
        if (sent_ == 0) return true;
        if (sent_ < 0) return false;
        left_bytes -= sent_;
        sent_bytes += sent_;
      }
      while (left_bytes > 0);
      sent = sent_bytes;

      return true;
    }
    
    auto is_open() -> bool
    {
      return (m_socket == -1) ? false : m_is_open;
    }

    auto last_error() -> uint32_t 
    {
      return m_last_error;
    }

  private:
    socket_type m_socket;
    bool        m_is_open;
    uint32_t    m_last_error;
  };

}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////

