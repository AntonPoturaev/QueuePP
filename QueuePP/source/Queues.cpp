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

#include <Queues++/queues.hpp>

#include "queue_implementation.hpp"
#include "main_thread_control_policy.hpp"
#include "background_thread_control_policy.hpp"
#include "store.hpp"

#include <cassert>

namespace Queues {

  namespace {
    std::string const gMainQueueName("Main");
    
    private::control_policy* _create_raw_policy_by_name(std::string const& name) {
      if (gMainQueueName == name) {
        return new private::main_thread_control_policy;
      } else {
        return new private::background_thread_control_policy(name);
      }
    }
    
  } ///< end unnamed namespace

  queue::~queue()
  {
    assert(_impl && "Bad data!");
    _impl->stop();
    CL::DeleteRawPtr(_impl);
    
  }

  void queue::start_routine()
  {
    assert(_impl && "Bad data!");
    _impl->start_routine();
  }

  void queue::stop()
  {
    assert(_impl && "Bad data!");
    _impl->stop();
  }

  bool queue::is_started() const
  {
    assert(_impl && "Bad data!");
    return _impl->is_started();  
  }

  void queue::post(task_t&& task)
  {
    assert(_impl && "Bad data!");
    _impl->post(std::forward<task_t>(task));
  }

  void queue::post(task_t const& task)
  {
    assert(_impl && "Bad data!");
    _impl->post(task);
  }

  std::thread::id queue::work_thread_id() const
  {
    assert(_impl && "Bad data!");
    return _impl->work_thread_id();
  }
  
  std::string const& queue::get_name() const {
    return _impl->get_name();
  }

  queue::ptr_t queue::create_or_get(std::string const& name) {
    auto instance = private::store::instance().get_by_name(name);
    
    if (!instance) {
      if (instance = ptr_t(new queue(name))) {
        if (!private::store::instance().add(instance)) {
          assert(!"failed to add new queue instance to store");
        }
      } else {
        assert(!"failed to create new instance of the queue");
      }
    }
    
    return instance;
  }

  queue::ptr_t queue::create_main_or_get() {
    return create_or_get(gMainQueueName);
  }
  
  queue::queue(std::string const& name)
    : _impl(new private::queue_implementation(name, private::control_policy::ptr_t(_create_raw_policy_by_name(name))))
  {
  }
  
  void queue::kill(std::string const& name) {
    ///TODO: Implement me ;)
  }
  
  void queue::kill(ptr_t const& q) {
    ///TODO: Implement me ;)
  }
 
} /// end namespace Queues
