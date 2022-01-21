#pragma once

#include "exception.h"

#include <winsock2.h>

////////////////////////////////////////////////////////////////////////////////
//
// 클라이언트 socket은 외부인터페이스가 
// 1. 표준 api
// 2. 표준 api의 buffer pointer(uint8_t*) 밖에 없다.
//    만약 proactor, reactor를 사용하면 소겟의 socket의 데이타타입등이 필요하겠지
//    socket_type handle();
//
////////////////////////////////////////////////////////////////////////////////
namespace net::detail::win32 {

  class socket
  {
  public:
    typedef SOCKET socket_type;

  public:
    socket() : m_socket(INVALID_SOCKET)
    {
      WSADATA wd;
      if (::WSAStartup(MAKEWORD(2, 2), &wd) == SOCKET_ERROR)
      {
        m_last_error = ::WSAGetLastError();
        // TODO
        // throw core_socket_creation_fail();
      }

      m_last_error = 0;
      m_is_open = false;

      m_async_event = ::WSACreateEvent();
      m_write_event = ::WSACreateEvent();
      m_read_event  = ::WSACreateEvent();
      m_socket = INVALID_SOCKET;
      if (invalid(m_async_event) || invalid(m_write_event) || invalid(m_read_event))
      {
        // TODO
        // throw core_socket_creation_fail();
      }
    }

    ~socket()
    {
      close();
    }

    auto open(char const* ip, uint16_t port, uint32_t timeout) -> bool
    {
      sockaddr_in serv;
      ZeroMemory(&serv, sizeof(serv));
      serv.sin_family = AF_INET;
      serv.sin_addr.s_addr = inet_addr(ip);
      serv.sin_port = htons(port);

      m_socket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
      if (m_socket == INVALID_SOCKET)
      {
        m_last_error = ::WSAGetLastError();
        return false;
      }

      if (::WSAConnect(m_socket, (sockaddr*)&serv, sizeof(serv), 0, 0, 0, 0) == SOCKET_ERROR)
      {
        bool res = false;
        int err = ::WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
        {
          WSAEVENT events[1];
          events[0] = m_async_event;
          DWORD ret = ::WSAWaitForMultipleEvents(1, events, FALSE, timeout, FALSE);
          switch(ret)
          {
            case WSA_WAIT_EVENT_0:
              return true;
            case WSA_WAIT_EVENT_0+1:
              m_last_error = ERROR_OPERATION_ABORTED;
              break;
            case WAIT_TIMEOUT:
              m_last_error = ERROR_TIMEOUT;
              break;
            default :
              m_last_error = ::WSAGetLastError();
              break;
          }
        }
        else
        {
          m_last_error = err;
        }

        ::closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return false;
      }

      m_is_open = true;
      return true;
    }

    auto close(bool hard_close=false) -> void
    {
      m_is_open = false;
      if (m_socket == INVALID_SOCKET)
        return;

      if (hard_close)
      {
        linger l = { 1, 0 };
        setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(linger));
      }

      if (::WSAGetLastError() != WSAENOTCONN)
        shutdown(m_socket, SD_SEND);

      ::closesocket(m_socket);
      m_socket = INVALID_SOCKET;
    }

    auto recv(byte_buffer& buffer, uint32_t to_read, uint32_t timeout=30*1000, bool hard_close=false) -> bool
    {
      int const BLOCK_SIZE = 1024;
      DWORD dwFlags = 0;
      DWORD dwBytes = 0;
      uint8_t  local_buffer[BLOCK_SIZE] = { 0 };

      m_last_error = 0;
      bool retval = true;

      WSAOVERLAPPED ov; ZeroMemory(&ov, sizeof(ov));
      ov.hEvent = m_read_event;

      uint32_t total_read = 0;
      uint32_t bytes_left = to_read;
      do
      {
        WSABUF buf;
        int buf_len = min(BLOCK_SIZE, bytes_left);
        buf.len = buf_len;
        buf.buf = (char*)local_buffer;

        if (WSARecv(m_socket, &buf, 1, &dwBytes, &dwFlags, &ov, NULL) == SOCKET_ERROR)
        {
          int err = ::WSAGetLastError();
          if (err != ERROR_IO_PENDING)
          {
            m_last_error = err;
            close(hard_close);
            return false;
          }
        }

        WSAEVENT events[1];
        events[0] = ov.hEvent;

        DWORD res = WSAWaitForMultipleEvents(1, events, FALSE, timeout, FALSE);
        switch(res)
        {
          case WSA_WAIT_EVENT_0:
            if (WSAGetOverlappedResult(m_socket, &ov, &dwBytes, TRUE, &dwFlags))
            {
              if (dwBytes > 0)
              {
                buffer.set_stream(local_buffer, dwBytes);
                total_read += dwBytes;
                bytes_left -= dwBytes;
              }
              else // return 0
              {
                m_last_error = ::WSAGetLastError();
                if (m_last_error == 0 || m_last_error == ERROR_IO_PENDING)
                {
                  WSAResetEvent(events[0]);
                  return true;
                }
                else
                {
                  retval = false;
                }
                break;
              }
            }
            else
            {
              m_last_error = ::WSAGetLastError();
              retval = false;
              break;
            }
            WSAResetEvent(events[0]);
            break;

          case WSA_WAIT_EVENT_0+1: // 강제종료 -> 구현 안함<
            break;
          case WAIT_TIMEOUT:
            m_last_error = ERROR_TIMEOUT;
            retval = false;
            break;
          default:
            m_last_error = ::WSAGetLastError();
            retval = false;
            break;
        }
      }
      while(retval && bytes_left>0);

      if (!retval)
      {
        close(true);
        return false;
      }

      return retval;
    }

    auto send(uint8_t* buffer, uint32_t to_send, uint32_t& sent, uint32_t timeout=30*1000, bool hard_close=false) -> bool
    {
      m_last_error = 0;
      bool retval  = false;
      WSAOVERLAPPED ov; ZeroMemory(&ov, sizeof(ov));
      ov.hEvent = m_write_event;

      WSABUF buf = { to_send, (char*)buffer };
      DWORD dwSent = 0;
      if (WSASend(m_socket, &buf, 1, &dwSent, 0, &ov, NULL) == SOCKET_ERROR)
      {
        int err = ::WSAGetLastError();
        if (err != ERROR_IO_PENDING)
        {
          m_last_error = err;
          close(hard_close);
          return false;
        }
      }
      sent = uint32_t(dwSent);

      WSAEVENT events[1];
      events[0] = ov.hEvent;
      DWORD ret = WSAWaitForMultipleEvents(1, events, FALSE, timeout, FALSE);
      switch(ret)
      {
        case WSA_WAIT_EVENT_0:
          retval = true;
          WSAResetEvent(events[0]);
          break;
        case WAIT_TIMEOUT:
          m_last_error = ERROR_TIMEOUT;
          break;
        default:
          m_last_error = ::WSAGetLastError();
          break;
      }

      if (!retval)
      {
        close(true);
        return false;
      }

      return retval;
    }
    
    auto is_open() -> bool
    {
      return (m_socket == INVALID_SOCKET) ? false : m_is_open;
    }

    auto last_error() -> uint32_t 
    {
      switch (m_last_error) 
      {
        case ERROR_TIMEOUT:           return PCL_TIMEOUT;
        case ERROR_OPERATION_ABORTED: return PCL_INTERNAL_ERROR;
        default:                      return m_last_error;
      }

      return PCL_OK;
    }

    // void SetWindow(HWND hWnd) { m_hWnd = hWnd; }
    // void SetProgressMode(bool mode = true);

  private:
    auto invalid(HANDLE h) const -> bool 
    {
      return h == WSA_INVALID_EVENT;
    }

    socket_type m_socket;
    bool     m_is_open;
    DWORD    m_last_error;
    uint32_t m_timeout;
    WSAEVENT m_write_event;    // Write()의 Overlapped 수행을 위한 이벤트
    WSAEVENT m_read_event;     // Read()의 Overlapped 수행을 위한 이벤트
    WSAEVENT m_async_event;    // Connect()의 Overlapped 수행을 위한 이벤트
  };

}
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////

