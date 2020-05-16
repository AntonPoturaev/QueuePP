include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/include
)

set(QUEUES_CPP_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/include/Queues++/queues.hpp
  ${CMAKE_CURRENT_LIST_DIR}/source/Queues.cpp
  ${CMAKE_CURRENT_LIST_DIR}/include/Queues++/MainQueue.hpp
  ${CMAKE_CURRENT_LIST_DIR}/source/main_queue.cpp
  ${CMAKE_CURRENT_LIST_DIR}/source/background_thread_control_policy.cpp
  ${CMAKE_CURRENT_LIST_DIR}/source/main_thread_control_policy.cpp
  ${CMAKE_CURRENT_LIST_DIR}/source/queue_implementation.cpp
  ${CMAKE_CURRENT_LIST_DIR}/source/store.cpp
  )

