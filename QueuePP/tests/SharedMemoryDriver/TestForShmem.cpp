#include <stdio.h>

#include "gtest/gtest.h"
#include "TestForSHmem.hpp"

#include <CL/Logger.hpp>
namespace {
  using namespace Luxoft::Connectivity::BtPlatform;
  void _InitLogger() {
    SET_LOGGING_INFORMATION_SETTINGS("BTPlatform", "bluetooth_launcher");
    CL::Logging::Logger::SetFilter("Luxoft::Connectivity::BtPlatform::");
  }
}

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");
  _InitLogger();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}