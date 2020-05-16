///////////////////////////////////////////////////////////////////////////////
///
/// Project          MIB3 Radio2/Connectivity
/// Copyright (c)    2017
/// Company          Luxoft
///                  All rights reserved
///
////////////////////////////////////////////////////////////////////////////////
/// @file            ClienForTest.cpp
/// @authors         viktor
/// @date            3/10/17
///
/// @brief           Implementation of class ClienForTest
///
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <thread>
#include <gtest/gtest.h>

#include <CL/SharedMemoryDriver/SharedMemoryDriver.hpp>
#include <CL/Logger.hpp>

#define SHMEM_NAME "SHmemN1"
#define SHMEM_SIZE 512
#define TEST_MESSAGE "I am not writing a test message"
#define TEST_MESSAGE_SIZE 31

namespace {
  using namespace Luxoft::Connectivity::BtPlatform;
  void _InitLogger() {
    SET_LOGGING_INFORMATION_SETTINGS("BTPlatform", "bluetooth_launcher");
    CL::Logging::Logger::SetFilter("Luxoft::Connectivity::BtPlatform::");
  }
}

namespace Luxoft { namespace Connectivity { namespace BtPlatform { namespace CL {//TODO Change namespaces

TEST(SHmemoryClient, CreateWriteFromMemory){
  SharedMemoryDriver shmemHandler(SHMEM_NAME, SHMEM_SIZE);

  tByte* addr = shmemHandler.GetShmemAdress();
  char message_from_memory[TEST_MESSAGE_SIZE];
  memcpy(addr, TEST_MESSAGE, TEST_MESSAGE_SIZE);
  std::this_thread::sleep_for(std::chrono::seconds(10));
  mempcpy(message_from_memory, addr, TEST_MESSAGE_SIZE);

  EXPECT_STRCASENE(TEST_MESSAGE, message_from_memory);
}

}}}} /// end namespace Luxoft::Connectivity::BtPlatform::CL

GTEST_API_ int main(int argc, char **argv) {
  _InitLogger();
  printf("Running main() from gtest_main.cc\n");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

