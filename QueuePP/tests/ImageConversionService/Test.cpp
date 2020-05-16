#include "Test.hpp"

#include <fstream>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <Data/Data.hpp>

namespace Bluetooth { namespace CL {

  namespace {
    namespace bfs = boost::filesystem;

    typedef boost::interprocess::file_mapping tFileMapping;
    typedef boost::interprocess::mapped_region tMappedRegion;

    const std::string patch = "../../../../../BtControl/";
  }

  Test::Test()
    : _counter(0)
    , _convertor(ImageConversionService::Instance().Acquire_JpgConvertor())
    , _connection(_convertor.lock()->AttachOnConversionComplete(
      std::bind(&Test::OnDataComplete, this, std::placeholders::_1, std::placeholders::_2))) {
  }

  Test::~Test() {
    std::unique_lock<std::mutex> lock(_access);
    _condition.wait(lock, [this] { return _counter == 0; });

    ImageConversionService::Instance().ReleaseConvertor(_convertor);
  }

  void Test::DoConvert(tIDataPtr &img, tIJpgConvertorPtr convertor) {
    std::lock_guard<std::mutex> lock(_access);
    ++_counter;
    convertor->Convert(img);
  }


  void Test::Run() {
    if (auto cnv = _convertor.lock()) {
      tIDataPtr img = FileToBuffer(patch + "CL/tests/ImageConversionService/refImg/fire.gif");
      DoConvert(img, cnv);

      for (size_t i = 0; i < 100; ++i) {
        DoConvert(img, cnv);
        std::cout << _counter << std::endl;
      }
	  std::cout << "Run test" << std::endl;
    }

    std::cout << "End of Run" << std::endl;
  }

  void Test::OnDataComplete(tIDataPtr big, tIDataPtr small) {
    std::cout << "Begin of DataComplete" << std::endl;
    BufferToFile(big, patch + "CL/tests/ImageConversionService/resizeImg/big.jpeg");
    BufferToFile(small, patch + "CL/tests/ImageConversionService/resizeImg/smal.jpeg");
    --_counter;
    std::cout << "End of DataComplete" << std::endl;
    std::cout << _counter << std::endl;///////////////////////////////
    _condition.notify_one();
  }

  tIDataPtr Test::FileToBuffer(const std::string& path) {
    tIDataPtr data;
    bfs::path srcFilePath(path);
    if (bfs::exists(srcFilePath) && bfs::is_regular_file(srcFilePath)) {
      tFileMapping const mapping(srcFilePath.c_str(), boost::interprocess::read_only);
      tMappedRegion region(mapping, boost::interprocess::read_only);
      std::vector<std::uint8_t> buffer(region.get_size(), 0);
      std::memcpy(&buffer[0], region.get_address(), region.get_size());
      data.reset(new Data(std::move(buffer)));
    }
    return data;
  }

  void Test::BufferToFile(const tIDataPtr& data, const std::string& path) {
    if (data->DataSize() > 0) {
      std::ofstream ofs(path, std::ios_base::binary | std::ios_base::out);
      if (ofs.is_open()) {
        ofs.rdbuf()->sputn(reinterpret_cast<const char*>(data->DataPtr()), data->DataSize());
        ofs.close();
      }
    }
  }


}} /// end namespace Luxoft::Connectivity::BtPlatform::CL
