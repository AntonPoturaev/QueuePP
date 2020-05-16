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

#include "queue_implementation.hpp"

#include <cassert>

namespace Queues { namespace details {

  namespace {
    class _scoped_bool
    {
    public:
      _scoped_bool(std::atomic_bool& value)
        : _value(value)
      {
        _value.store(true, std::memory_order_relaxed);
      }

      ~_scoped_bool() {
        _value.store(false, std::memory_order_relaxed);
      }

    private:
      std::atomic_bool& _value;
    };

  } /// end unnamed namespace

  queue_implementation::queue_implementation(std::string const& name, control_policy::ptr_t&& ctrl)
    : _name(name)
    , _stop(false)
    , _started(false)
    , _ctrl(std::forward<control_policy::ptr_t>(ctrl))
  {
    assert(_ctrl && "Bad data!");
  }

  queue_implementation::~queue_implementation() {
    stop();
  }

  void queue_implementation::start_routine()
  {
    assert(_ctrl && "Bad data!");
    _ctrl->start([this](){ _routine(); });
  }

  void queue_implementation::stop()
  {
    if (!_stoped())
    {
      lock_t const lock(_access);

      _store.clear();
      _stop.store(true, std::memory_order_relaxed);
      _cond.notify_one();

      assert(_ctrl && "Bad data!");
      _ctrl->stop();
    }
  }

  bool queue_implementation::is_started() const {
    return _started.load(std::memory_order_relaxed);
  }

  void queue_implementation::post(task_t&& task)
  {
    lock_t const lock(_access);
    _store.push_back(std::forward<task_t>(task));
    _cond.notify_one();
  }

  void queue_implementation::post(task_t const& task)
  {
    lock_t const lock(_access);
    _store.push_back(task);
    _cond.notify_one();
  }

  std::thread::id queue_implementation::work_thread_id() const 
  { /// TODO: need lock?
    assert(is_started() && "Bad logick!");
    return _id;
  }
  
  std::string const& queue_implementation::get_name() const {
    return _name;
  }

  void queue_implementation::_routine()
  {
    _id = std::this_thread::get_id();
    _scoped_bool const start_scope(_started);

    while (!_stoped())
    {
      task_t task;

      {
        lock_t lock(_access);
        while (_store.empty())
        {
          if (_stoped())
            return;

          _cond.wait(lock);
        }
      }
      
      task = std::move(_store.front());
      _store.pop_front();

      try 
      {
        if (task)
          task();
      }
      catch (...) {}
    }
  }

  bool queue_implementation::_stoped() {
    return _stop.load(std::memory_order_relaxed);
  }
     
}} /// end namespace Queues::details
