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

#ifndef __QUEUES_PLUS_PLUS_DETAILS_STORE_HPP_
# define __QUEUES_PLUS_PLUS_DETAILS_STORE_HPP_

#include <Queues++/queues.hpp>

#include <mutex>

#include <boost/unordered_set.hpp>
#include <boost/noncopyable.hpp>

namespace boost {
    std::size_t hash_value(Queues::queue::ptr_t const& q);
} /// end namespace boost 

namespace Queues { namespace details {

  class store : public boost::noncopyable {
    typedef boost::unordered_set<queue::ptr_t> _store_t;
  public:
    ~store();
    
    static store& instance();
    
    queue::ptr_t get_by_name(std::string const& name) const;
    bool add(queue::ptr_t const& queue);
    bool add(queue::ptr_t&& queue);
    
    void remove(queue::ptr_t const& queue);
    void remove(std::string const& name);
    
  private:
    store() = default;
    
    _store_t::const_iterator _find_by_name(std::string const& name) const;
    
  private:
    _store_t _store;
    mutable std::mutex _access;
  };

}} /// end namespace Queues::details

#endif /// !__QUEUES_PLUS_PLUS_DETAILS_STORE_HPP_
