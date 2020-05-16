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

#ifndef __QUEUES_PLUS_PLUS_QUEUES_HPP_
# define __QUEUES_PLUS_PLUS_QUEUES_HPP_

#include <CL/Queues/types.hpp>

#include <functional>
#include <thread>
#include <memory>
#include <string>

namespace Queues {

  namespace details { class queue_implementation; }
  
  class queue
  {
  public:
    typedef std::shared_ptr<queue> ptr_t;

  public:
    ~queue();

    void start_routine();
    void stop();
    bool is_started() const;

    void post(task_t&& task);
    void post(task_t const& task);

    std::thread::id work_thread_id() const;

    std::string const& get_name() const;
    
    static ptr_t create_or_get(std::string const& name);
    static ptr_t create_main_or_get();
    
    /// @brief remove queue ptr object from internal store
    /// TODO: can not call queue::stop() !!!
    static void kill(std::string const& name);
    static void kill(ptr_t const& q);

  private:
    queue(std::string const& name);

  private:
    private::queue_implementation* _impl;
  };
  
} /// end namespace Queues

#endif /// !__QUEUES_PLUS_PLUS_QUEUES_HPP_
