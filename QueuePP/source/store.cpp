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

#include "store.hpp"

#include <cassert>
#include <algorithm>

#include <boost/bind.hpp>
#include <boost/functional/hash.hpp>

namespace boost {
  std::size_t hash_value(Queues::queue::ptr_t const& q) {
    assert(q && "Bad data!");
    return hash_value(q->get_name());
  }
} /// end namespace boost 

namespace Queues { namespace details {

  namespace {
    typedef std::lock_guard<std::mutex> const _lock_t;    
  } /// end unnamed namespace

  store::~store() {
    _lock_t lock(_access);
    _store.clear();
  }
  
  store& store::instance() {
    static store instance;
    return instance;
  }
  
  queue::ptr_t store::get_by_name(std::string const& name) const {
    _lock_t lock(_access);
    auto const found = _find_by_name(name);
    return _store.end() == found ? nullptr : *found;
  }
  
  bool store::add(queue::ptr_t const& q) {
    _lock_t lock(_access);
    assert(q && "Bad data!");
    return _store.insert(q).second;
  }
  
  bool store::add(queue::ptr_t&& q) {
    _lock_t lock(_access);
    assert(q && "Bad data!");
    return _store.insert(std::forward<queue::ptr_t>(q)).second;
  }
  
  void store::remove(queue::ptr_t const& queue) {
    _lock_t lock(_access);
    assert(queue && "Bad data!");
    remove(queue->get_name());
  }
  
  void store::remove(std::string const& name) {
    _lock_t lock(_access);
    _store.erase(_find_by_name(name));
  }
  
  store::_store_t::const_iterator store::_find_by_name(std::string const& name) const {
    /// TODO: rewrite this
    return std::find_if(_store.begin(), _store.end(), boost::bind(&queue::get_name, _1) == name);
  }  

}} /// end namespace Queues::details
