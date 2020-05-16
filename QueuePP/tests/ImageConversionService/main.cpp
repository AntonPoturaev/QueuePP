
#include <mutex>
#include <thread>
#include <condition_variable>

#include <iostream>
#include <functional>
#include <atomic>

#include "CL/ImageConversionService/ImageConversionService.hpp"
#include "Test.hpp"

using namespace std;
using namespace Bluetooth::CL;
#if 0

#endif

int main() {
  Test test;
  test.Run();
  test.Run();
  test.Run();
  return 0;
}

