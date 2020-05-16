#include <CL/Logger.hpp>
#include <CL/types.hpp>
#include <iostream>
#include <boost/format.hpp>

namespace {

  using namespace Bluetooth;
  void _InitLogger() {
    SET_LOGGING_INFORMATION_SETTINGS("BTPlatform", "bluetooth_launcher");
    CL::Logging::Logger::SetFilter("Bluetooth::");
  }
}

int main() {
  _InitLogger();

  {
    std::string macStr = "ac:c1:ee:46:cb:d8";
    CL::tMacAddress macFrom = CL::MacFromString(macStr, false);
    std::string macResult = CL::MacToString(macFrom, false);

    assert(macResult == macStr && "!EQ");
  }

  
  {
    std::string macStr = "ac:c1:ee:46:cb:d8";
    CL::tMacAddress macFrom = CL::MacFromString(macStr, true);
    std::string macResult = CL::MacToString(macFrom, true);

    assert(macResult == macStr && "!EQ");
  }

  return 0;
}

