#pragma once

#include <memory>

////////////////////////////////////////////////////////////////////////////////
//
// http://brotherst-code.blogspot.com/2015/09/the-pimpl-idiom-youre-doing-it-wrong.html
//
////////////////////////////////////////////////////////////////////////////////
namespace util {

  template<typename T>
  class pimpl_ptr
  {
  public:
    pimpl_ptr(T* impl = nullptr) 
      : impl(impl) { }

    pimpl_ptr(pimpl_ptr const&) = delete;
    pimpl_ptr& operator=(pimpl_ptr const&) = delete;

    pimpl_ptr(pimpl_ptr&& other) 
      : impl(other.impl) 
    {
      other.impl = nullptr;
    }

    pimpl_ptr& operator=(pimpl_ptr&& other) 
    {
      impl = other.impl;
      other.impl = nullptr;
      return *this;
    }

    ~pimpl_ptr() 
    {
      delete impl;
    }

    T const& operator*() const 
    {
      return *impl;
    }

    T& operator*() 
    {
      return *impl;
    }

    T const* operator->() const 
    {
      return impl;
    }

    T* operator->() 
    {
      return impl;
    }

  private:
    T* impl;
  };

} 
