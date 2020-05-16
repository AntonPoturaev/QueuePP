///////////////////////////////////////////////////////////////////////////////
///
/// Project           Queue++
/// Copyright (c)     2016
///
////////////////////////////////////////////////////////////////////////////////
/// @file            types.hpp
/// @authors         Poturaev Anton
/// @date            11.02.2016
///
/// @brief           Implementation of ...
///
////////////////////////////////////////////////////////////////////////////////

#include "main_thread_control_policy.hpp"

#include <cassert>

namespace Queues { namespace details {

  void main_thread_control_policy::start(routine_proc_t&& routine)
  {
    assert(routine && "Bad data!");
    if (routine) {
      routine();
    }
  }

  void main_thread_control_policy::stop() {}
  
}} /// end namespace Queues::details
