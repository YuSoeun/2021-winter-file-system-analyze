#pragma once

#include "handle.hpp"

#include <sqlite3.h>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace api::sqlite3 {

  struct ConnectionHandleTraits
  {
    using pointer = ::sqlite3*;

    static auto invalid() noexcept
    {
      return nullptr;
    }

    static auto close(pointer value) noexcept
    {
      assert(::sqlite3_close(value) == SQLITE_OK);
    }
  };

  using connection_handle 
    = utility::handle::unique_handle<ConnectionHandleTraits>;

} 

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
