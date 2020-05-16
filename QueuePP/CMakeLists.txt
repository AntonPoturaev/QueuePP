cmake_minimum_required(VERSION 2.8)
project(Queue)

include(Queues++.cmake)

# TODO: What is this?
# Add definitions for build CL libraries
#add_definitions(-DSIGNED_RIGHT_SHIFT_IS=1)
#add_definitions(-DARITHMETIC_RIGHT_SHIFT=1)
#add_definitions(-DSQLITE_THREADSAFE=2)

#add_subdirectory(tests)

add_library(Queues
  STATIC
  ${QUEUES_CPP_SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# TODO: find boost here

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "Boost is not configured")
endif()

target_link_libraries(
  ${Boost_LIBRARIES}
  pthread
  Queues
  CL
  )