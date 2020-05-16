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

#ifndef __QUEUES_PLUS_PLUS_TYPES_HPP_
# define __QUEUES_PLUS_PLUS_TYPES_HPP_

#include <function>

namespace Queues {

  typedef std::function<void()> task_t;
   
} /// end namespace Queues

#endif /// !__QUEUES_PLUS_PLUS_TYPES_HPP_
