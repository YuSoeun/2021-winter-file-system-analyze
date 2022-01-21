#pragma once

#include <boost/circular_buffer.hpp>
#include <condition_variable>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
template <class T>
class BoundedBuffer
{
public:
  typedef boost::circular_buffer<T> container_type;
  typedef typename container_type::size_type  size_type;
  typedef typename container_type::value_type value_type;

  explicit BoundedBuffer(size_type capacity) 
    : m_unread(0), m_container(capacity) 
  {}

  BoundedBuffer(const BoundedBuffer&) = delete;

  auto count() -> int
  {
    return m_unread;
  }

  auto push_front(const value_type& item) -> bool
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    m_while_full.wait(lock,
      [this] { return m_unread < m_container.capacity() || m_shutdown; });

    if (m_shutdown)
      return false;

    m_container.push_front(item);
    ++m_unread;
    lock.unlock();
    m_while_empty.notify_all();

    return true;
  }

  auto pop_back(value_type* pItem) -> bool
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    m_while_empty.wait(lock, [this] { return m_unread > 0 || m_shutdown; }); 

    if (m_shutdown)
      return false;

    *pItem = m_container[--m_unread];
    lock.unlock();
    m_while_full.notify_all();

    return true;
  }

  auto resize(int size) -> void
  {
    m_container.resize(size, T());
  }

  auto clear() -> void
  {
    m_container.clear();
    m_unread = 0;
  }

  auto shutdown() -> void
  {
    m_shutdown = true;
    m_while_full.notify_all();
    m_while_empty.notify_all();
  }

private:
  bool m_shutdown {false};
  size_type m_unread;
  container_type m_container;
  std::mutex     m_mutex;
  std::condition_variable m_while_empty;
  std::condition_variable m_while_full;
};

