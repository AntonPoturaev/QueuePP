///////////////////////////////////////////////////////////////////////////////
///
/// Project          MIB3 Radio2/Connectivity
/// Copyright (c)    2017
/// Company          Luxoft
///                  All rights reserved
///
////////////////////////////////////////////////////////////////////////////////
/// @file            GTestCacheService.hpp
/// @authors         SLalym
/// @date            24/02/17
///
/// @brief           Implementation of class GTestCacheService
///
////////////////////////////////////////////////////////////////////////////////

#include "Test.hpp"

#include <fstream>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "../CacheService/Data.hpp"

#include <gtest/gtest.h>

#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#include <boost/gil/extension/io/jpeg_all.hpp>
#include <boost/gil/extension/io/tiff_all.hpp>
#include <boost/gil/extension/io/png_all.hpp>
#include <boost/gil/extension/io/bmp_all.hpp>

#include <boost/iostreams/stream.hpp>
#include <../../source/CL/ImageConversionService/container_device.hpp>

namespace Luxoft { namespace Connectivity { namespace BtPlatform { namespace CL {

  namespace {
    namespace bfs = boost::filesystem;

    typedef boost::interprocess::file_mapping tFileMapping;
    typedef boost::interprocess::mapped_region tMappedRegion;
  }

  using string_device = boost::iostreams::example::container_device<std::vector<char>>;

  class ImageConversionServiceTest : public ::testing::Test {
   public:
    void OnDataComplete(tIDataPtr big, tIDataPtr small) {
      _big = big;
      _small = small;
      --_counter;
      _condition.notify_one();
    }

    boost::gil::rgb8_image_t ReadJpgImg(tDataPtr& img) {
      using ImageTypes = boost::mpl::vector<
        boost::gil::gray8_image_t, boost::gil::cmyk8_image_t, boost::gil::rgb8_image_t,
        boost::gil::rgba8_image_t, boost::gil::gray16_image_t, boost::gil::rgb16_image_t,
        boost::gil::rgba16_image_t, boost::gil::gray32f_image_t, boost::gil::rgb32f_image_t>;

      using tAnyImage = boost::gil::any_image<ImageTypes>;
      tAnyImage tmp;

      boost::iostreams::stream<string_device> io(*reinterpret_cast<std::vector<char>*>(&img->_data));
      boost::gil::read_image(io, tmp, boost::gil::jpeg_tag());
      boost::gil::rgb8_image_t runtime_image(tmp.width(), tmp.height());
      return runtime_image;
    }

    void ReadImage(tDataPtr& img, boost::gil::rgb8_image_t& runtime_image) {
      using ImageTypes = boost::mpl::vector<
        boost::gil::gray8_image_t, boost::gil::cmyk8_image_t, boost::gil::rgb8_image_t,
        boost::gil::rgba8_image_t, boost::gil::gray16_image_t, boost::gil::rgb16_image_t,
        boost::gil::rgba16_image_t, boost::gil::gray32f_image_t, boost::gil::rgb32f_image_t>;

      using tAnyImage = boost::gil::any_image<ImageTypes>;

      tAnyImage tmp;
      boost::iostreams::stream<string_device> io(*reinterpret_cast<std::vector<char>*>(&img->_data));
      if (img->_data[0] == 0xFF) {
        if (img->_data[1] == 0xD8 && img->_data[2] == 0xFF) {
          boost::gil::read_image(io, tmp, boost::gil::jpeg_tag());
        } else {
          return;
        }
      } else {
        if (img->_data[0] == 0x89) {
          if (img->_data[1] == 0x50 && img->_data[2] == 0x4E && img->_data[3] == 0x47) {
            boost::gil::read_image(io, tmp, boost::gil::png_tag());
          } else {
            return;
          }
        }
      }
      runtime_image.recreate(tmp.width(), tmp.height());
      boost::gil::copy_and_convert_pixels(const_view(tmp), view(runtime_image));
    }

   protected:
    virtual void SetUp() {
      _counter = 0;
      convertor = ImageConversionService::Instance().Acquire_JpgConvertor();
    }

    virtual void TearDown() {
    }


    tIDataPtr FileToBuffer(const std::string& path) {
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

   protected:
    tIDataPtr _big, _small;
    tIJpgConvertorWeak convertor;
    std::atomic<std::size_t> _counter;
    std::mutex _access;
    std::condition_variable _condition;
  };

  TEST_F(ImageConversionServiceTest, Positive_test) {
    boost::gil::rgb8_image_t runtime_image;

    convertor.lock()->AttachOnConversionComplete(std::bind(&ImageConversionServiceTest::OnDataComplete, this, std::placeholders::_1, std::placeholders::_2));

    tIDataPtr img = FileToBuffer("../../../../CL/tests/ImageConversionService/refImg/artleo.com_1272.jpg");

    if (auto cnv = convertor.lock()) {
      ++_counter;
      cnv->Convert(img);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));//Need to see all signals


    EXPECT_TRUE(_big.get());
    EXPECT_TRUE(_small.get());

    tDataPtr db = std::static_pointer_cast<Data>(_big);

    ReadImage(db, runtime_image);


    if (runtime_image.height() == 150 || runtime_image.width() == 150) {
      EXPECT_TRUE(1);
    } else {
      EXPECT_TRUE(0);
    }

    tDataPtr ds = std::static_pointer_cast<Data>(_small);

    ReadImage(ds, runtime_image);

    if (runtime_image.height() == 32 || runtime_image.width() == 32) {
      EXPECT_TRUE(1);
    } else {
      EXPECT_TRUE(0);
    }
    std::unique_lock<std::mutex> lock(_access);
    _condition.wait(lock, [this] { return _counter == 0; });
  }

  TEST_F(ImageConversionServiceTest, tt) {

  }

}}}} /// end namespace Luxoft::Connectivity::BtPlatform::CL

int main(int argc, char** argv) {
  using namespace Luxoft::Connectivity::BtPlatform::CL;
  std::cout << "Hello" << std::endl;

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

