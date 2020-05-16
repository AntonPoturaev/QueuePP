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

#ifndef __QUEUES_PLUS_PLUS_DETAILS_QUEUE_IMPLEMENTATION_HPP_
# define __QUEUES_PLUS_PLUS_DETAILS_QUEUE_IMPLEMENTATION_HPP_

#include "control_policy.hpp"

#include <deque>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

namespace Queues { namespace details {

  class queue_implementation
  {
  public:
    typedef std::unique_lock<std::mutex> lock_t;

  public:
    queue_implementation(std::string const& name, control_policy::ptr_t&& ctrl);

    ~queue_implementation();

    void start_routine();
    void stop();
    bool is_started() const;

    void post(task_t&& task);
    void post(task_t const& task);
    
    std::thread::id work_thread_id() const; 
    std::string const& get_name() const;

  private:
    void _routine();
    bool _stoped();

  private:
    std::string const _name;
    std::deque<task_t> _store;
    mutable std::mutex _access;
    std::condition_variable _cond;
    std::atomic_bool _stop;
    std::atomic_bool _started;
    std::thread::id _id;
    control_policy::ptr_t _ctrl;  
  };
     
}} /// end namespace Queues::details

#endif /// !__QUEUES_PLUS_PLUS_DETAILS_QUEUE_IMPLEMENTATION_HPP_
