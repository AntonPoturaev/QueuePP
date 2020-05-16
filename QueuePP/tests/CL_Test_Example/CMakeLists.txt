add_executable(CL_Test_Example
  CL_Test_Example.cpp)

find_package(Boost COMPONENTS
        system
        filesystem
        REQUIRED)

target_link_libraries(CL_Test_Example
        ${Boost_LIBRARIES}
        ${CL_LIBRARIES}
        Logging
)