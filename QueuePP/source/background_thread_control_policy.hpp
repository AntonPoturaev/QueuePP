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

#ifndef __QUEUES_PLUS_PLUS_DETAILS_BACKGROUND_THREAD_CONTROL_POLICY_HPP_
# define __QUEUES_PLUS_PLUS_DETAILS_BACKGROUND_THREAD_CONTROL_POLICY_HPP_

#include "control_policy.hpp"

#include <thread>

namespace Queues { namespace details {

  class background_thread_control_policy
    : public control_policy
  {
  public:
    virtual ~background_thread_control_policy();
    background_thread_control_policy(std::string const& name);

    virtual void start(routine_proc_t&& routine) override final;
    virtual void stop() override final;

  private:
    void _stop();

  private:
    std::string const _name;
    std::unique_ptr<std::thread> _worker;
  };

}} /// end namespace Queues::details

#endif /// !__QUEUES_PLUS_PLUS_DETAILS_BACKGROUND_THREAD_CONTROL_POLICY_HPP_
