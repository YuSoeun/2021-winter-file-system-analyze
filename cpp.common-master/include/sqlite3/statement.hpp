#pragma once

#include "statement_handle.hpp"
#include "connection.hpp"

#include <cstring>
#include <string>

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
namespace api::sqlite3 {

  struct statement
  {
    auto prepare(connection const& c, char const* text) -> bool
    {
      m_handle.reset();

      return ::sqlite3_prepare_v2(c.m_handle.get(),
          text,
          ::strlen(text),
          m_handle.get_address_of(),
          nullptr) == SQLITE_OK;
    }

    auto bind(int const index, int const value) -> bool
    {
      return ::sqlite3_bind_int(m_handle.get(), index, value) == SQLITE_OK;
    }

    auto bind(int const index, char const* value) -> bool
    {
      return ::sqlite3_bind_text(m_handle.get(), index, value, 
          ::strlen(value), SQLITE_STATIC) == SQLITE_OK;
    }

    auto step() -> bool
    {
      auto result = ::sqlite3_step(m_handle.get());

      if (result == SQLITE_ROW)  return true;
      if (result == SQLITE_DONE) return false;

      return false;
    }

    auto reset() -> bool
    {
      return ::sqlite3_reset(m_handle.get()) == SQLITE_OK;
    }

    // class reader 
    auto get_int(int const col=0) -> int
    {
      return ::sqlite3_column_int(m_handle.get(), col);
    }

    auto get_int64(int const col=0) -> int64_t
    {
      return (int64_t)::sqlite3_column_int64(m_handle.get(), col);
    }

    auto get_string(int const col) -> std::string
    {
      return (char*)::sqlite3_column_text(m_handle.get(), col);
    }

  private:
    statement_handle m_handle;
  };

}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
