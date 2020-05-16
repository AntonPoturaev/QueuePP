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

#include "background_thread_control_policy.hpp"

#include <cassert>

namespace Queues { namespace details {

  background_thread_control_policy::~background_thread_control_policy() {
    _stop();
  }
  
  background_thread_control_policy::background_thread_control_policy(std::string const& name)
    : _name(name)
  {}

  void background_thread_control_policy::start(routine_proc_t&& routine)
  {
    assert(routine && "Bad data!");
    if (routine) {
      _worker.reset(std::thread(std::forward<routine_proc_t>(routine)));
    }
  }

  void background_thread_control_policy::stop() {
    _stop();
  }

  void background_thread_control_policy::_stop() {
    if (_worker) {
      if (_worker->joinable()) {
        _worker->join();
      }

      _worker.reset();
    }
  }

}} /// end namespace Queues::details
