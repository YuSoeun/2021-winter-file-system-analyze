#pragma once

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

//////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////// 
namespace sys {

  class threadpool
  {
  public:
    threadpool(std::size_t init_size=5)
      : m_init_thread(init_size)
      , m_work(m_scheduler)
    {}

    threadpool() = delete;
    threadpool(const threadpool&) = delete;

    virtual ~threadpool()
    {
      stop();
    }

    void start()
    {
      using namespace boost;
      for (uint8_t i=0; i<m_init_thread; ++i)
        m_threads.create_thread(bind(&boost::asio::io_service::run, &m_scheduler));
    }

    void stop()
    {
      m_scheduler.stop();
      m_threads.interrupt_all();
      m_threads.j       unix | utf-8 | cpp   35%   40:17
 15   public:oin_all();
    }

    void pool_size(std::size_t size)
    {
      m_init_thread = size;
    }

    template<typename Handler>
    void post(Handler h) { m_scheduler.post(h); }

    boost::asio::io_service& scheduler()
    {
      return m_scheduler;
    }

  private:
    std::size_t m_init_thread;

  private:
    boost::thread_group m_threads;
    boost::asio::io_service m_scheduler;
    boost::asio::io_service::work m_work;
  };

}

//////////////////////////////////////////////////////////////////////////////
//
// 
//
//////////////////////////////////////////////////////////////////////////////

#if 0
#include <iostream>
#include <boost/thread/mutex.hpp>
#include <boost/timer.hpp>

#include "asio_threadpool.h"

using namespace std;
using namespace boost;

mutex io_mutex;


void hello(int value)
{
  { mutex::scoped_lock l(io_mutex);
    cout << "thead id: " << this_thread::get_id() << " ";
    cout << "hello value : " << value << "\n";
    cout.flush();
    // this_thread::sleep(posix_time::milliseconds(10));
  }
}

int main(int argc, char const* argv[])
{
  sys::threadpool pool(5);
  pool.start();

  int const data_count = 1000;
  for (int i=0; i<data_count; i++) 
    pool.post(bind(hello, i));

  cout << "made " << data_count << "\n";

  sleep(5);

  return 0;
}

#endif
