#pragma once

#include <functional>
#include <iostream>

using namespace std;
////////////////////////////////////////////////////////////////////////////////
//
// Notice Arg should be a struct when we need arguments more thant 2
//                   or ...Arg, new c++ grammar
//
////////////////////////////////////////////////////////////////////////////////
template <typename R, typename Arg>
class AsyncNotifiable
{
public:
  typedef std::function<R(Arg)> ResultF;

public:
  template <typename Func>
  void result_notifier(Func f) { m_result_notifier = f; }

  R notify_result(Arg const& arg) 
  {
    return m_result_notifier ? m_result_notifier(arg) : R();
  }

private:
  ResultF m_result_notifier;
};

template <typename R>
class AsyncNotifiable<R, void>
{
public:
  typedef std::function<R()> ResultF;

public:
  template <typename Func>
  void result_notifier(Func f) { m_result_notifier = f; }

  R notify_result() 
  { 
    return m_result_notifier ? m_result_notifier() : R(); 
  }

private:
  ResultF m_result_notifier;
};

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
