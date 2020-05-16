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

#ifndef __QUEUES_PLUS_PLUS_DETAILS_CONTROL_POLICY_HPP_
# define __QUEUES_PLUS_PLUS_DETAILS_CONTROL_POLICY_HPP_

#include <CL/Queues/types.hpp>

#include <memory>

namespace Queues { namespace details {

  struct control_policy
  {
  public:
    typedef task_t routine_proc_t;
    typedef std::unique_ptr<control_policy> ptr_t;

  public:
    virtual ~control_policy() = default;
    
    virtual void start(routine_proc_t&& routine) = 0;
    virtual void stop() = 0;
  };

}} /// end namespace Queues::details

#endif /// !__QUEUES_PLUS_PLUS_DETAILS_CONTROL_POLICY_HPP_
