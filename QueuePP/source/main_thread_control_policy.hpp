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
/// @brief           Declaration of ...
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __QUEUES_PLUS_PLUS_DETAILS_MAIN_THREAD_CONTROL_POLICY_HPP_
# define __QUEUES_PLUS_PLUS_DETAILS_MAIN_THREAD_CONTROL_POLICY_HPP_

#include "control_policy.hpp"

namespace Queues { namespace details {

  struct main_thread_control_policy
    : public control_policy
  {
    virtual void start(routine_proc_t&& routine) override final;
    virtual void stop() override final;
  };

}} /// end namespace Queues::details

#endif /// !__QUEUES_PLUS_PLUS_DETAILS_MAIN_THREAD_CONTROL_POLICY_HPP_
