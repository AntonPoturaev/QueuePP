#ifndef TEST_H_
#define TEST_H_

#include <mutex>
#include <thread>
#include <condition_variable>

#include <iostream>
#include <functional>
#include <atomic>

#include <CL/ImageConversionService/ImageConversionService.hpp>
#include <CL/Data/IData.hpp>

namespace Bluetooth { namespace CL {


  class Test {

   public:
    Test();

    ~Test();

    void Run();

   private:
    void OnDataComplete(tIDataPtr big, tIDataPtr small);

    tIDataPtr FileToBuffer(const std::string &path);

    void BufferToFile(const tIDataPtr &data, const std::string &path);

    void DoConvert(tIDataPtr &img, tIJpgConvertorPtr convertor);

   private:
    std::atomic<std::size_t> _counter;
    std::mutex _access;
    std::condition_variable _condition;
    tIJpgConvertorWeak _convertor;
    boost::signals2::scoped_connection _connection;
  };


}} /// end namespace Luxoft::Connectivity::BtPlatform::C

#endif // TEST_H_
