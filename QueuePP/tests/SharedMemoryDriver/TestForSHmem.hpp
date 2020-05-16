///////////////////////////////////////////////////////////////////////////////
///
/// Project          MIB3 Radio2/Connectivity
/// Copyright (c)    2017
/// Company          Luxoft
///                  All rights reserved
///
////////////////////////////////////////////////////////////////////////////////
/// @file            TestForSHmem.hpp
/// @authors         VLatsyn
/// @date            3/10/17
///
/// @brief           Test of class SHmem
///
////////////////////////////////////////////////////////////////////////////////

#ifndef SHMEMCLASS_TESTFORSHMEM_HPP
#define SHMEMCLASS_TESTFORSHMEM_HPP

#define SHMEM_NAME "SHmemN1"
#define SHMEM_SIZE 512
#define TEST_MESSAGE "I am not writing a test message"
#define TEST_MESSAGE_SIZE 31

#include <CL/SharedMemoryDriver/SharedMemoryDriver.hpp>
#include <iostream>

namespace Luxoft { namespace Connectivity { namespace BtPlatform { namespace CL {//TODO Change namespaces

TEST(SHmemoryTest, createSHmemory){
  SharedMemoryDriver shmemHandler(SHMEM_NAME, SHMEM_SIZE);
  std::cout << "Created shmem , name = " << SHMEM_NAME << ", size = " << SHMEM_SIZE << std::endl;
}

TEST(SHmemoryTest, accessToMemory){
  SharedMemoryDriver shmemHandler(SHMEM_NAME, SHMEM_SIZE);

  const tByte* addr = shmemHandler.GetShmemAdress();
  ASSERT_NE(addr, MAP_FAILED);
}

TEST(SHmemoryTest, readFromMemory){
  SharedMemoryDriver shmemHandler(SHMEM_NAME, SHMEM_SIZE);

  const tByte* addr = shmemHandler.GetShmemAdress();
  char message_from_memory[TEST_MESSAGE_SIZE];
  mempcpy(message_from_memory, addr, TEST_MESSAGE_SIZE);
  std::cout << "origin  :" << TEST_MESSAGE << std::endl;
  std::cout << "shmemory:" << message_from_memory << std::endl;
  EXPECT_STRCASEEQ(TEST_MESSAGE, message_from_memory);
}

TEST(SHmemoryTest, writeToMemore){
  SharedMemoryDriver shmemHandler(SHMEM_NAME, SHMEM_SIZE);

  tByte* addr = shmemHandler.GetShmemAdress();
  memset(addr, '\0', TEST_MESSAGE_SIZE);
  ASSERT_NE(addr, MAP_FAILED);
}

}}}} /// end namespace Luxoft::Connectivity::BtPlatform::CL

#endif //SHMEMCLASS_TESTFORSHMEM_HPP
