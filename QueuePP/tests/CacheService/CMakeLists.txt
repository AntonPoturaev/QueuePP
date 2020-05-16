include_directories(../../include/)

add_executable(
  TestCacheService
  TestCacheService.cpp
  )

target_link_libraries(TestCacheService
  CacheService
  pthread
  ${CL_LIBRARIES}
  ${Boost_LIBRARIES}
  CL
  Logging
  )