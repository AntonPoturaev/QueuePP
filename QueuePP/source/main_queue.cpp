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

#include <CL/Queues/MainQueue.hpp>

#include <cassert>

namespace Queues { namespace Main {

  namespace {
    class _queue_holder
    {
    public:
      static _queue_holder& instance()
      {
        static _queue_holder instance;
        return instance;
      }

      queue& get_queue() {
        return *_queue;
      }

    private:
      _queue_holder()
        : _queue(queue::create_main_or_get())
      {
        assert(_queue && "Bad data!");
      }

    private:
      queue::ptr_t _queue;
    };
  
  } /// end unnamed namespace

  void start_routine() {
    _queue_holder::instance().get_queue().start_routine();
  }

  void stop() {
    _queue_holder::instance().get_queue().stop();
  }

  bool is_started() {
    return _queue_holder::instance().get_queue().is_started();
  }

  void post(task_t&& task) {
    _queue_holder::instance().get_queue().post(std::forward<task_t>(task));
  }

  void post(task_t const& task) {
    _queue_holder::instance().get_queue().post(task);
  }
 
}} /// end namespace Queues::Main
