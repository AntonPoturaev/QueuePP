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

#ifndef __QUEUES_PLUS_PLUS_MAIN_QUEUE_HPP_
# define __QUEUES_PLUS_PLUS_MAIN_QUEUE_HPP_

#include <CL/Queues/Queues.hpp>

namespace Queues { namespace Main {

  void start_routine();
  void stop();
  bool is_started();

  void post(task_t&& task);
  void post(task_t const& task);
 
}} /// end namespace Queues::Main

#endif /// !__QUEUES_PLUS_PLUS_MAIN_QUEUE_HPP_
