cmake_minimum_required(VERSION 3.6)
project(TestForShmem)

set(CMAKE_CXX_STANDARD 11)

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIR})
#include_directories(SHmemHandler/include)

#add_subdirectory(SHmemHandler/source/SHmemHandler)
set(SOURCE_FILES
  TestForShmem.cpp
  )
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_executable(ClienForTest ClientForTest.cpp)

target_link_libraries(ClienForTest
  SharedMemoryDriver
  gtest
  pthread
  rt
  )

target_link_libraries(${PROJECT_NAME}
  SharedMemoryDriver
  gtest
  pthread
  rt
  )

