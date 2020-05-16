///////////////////////////////////////////////////////////////////////////////
///
/// Project          MIB3 Radio2/Connectivity
/// Copyright (c)    2017
/// Company          Luxoft
///                  All rights reserved
///
////////////////////////////////////////////////////////////////////////////////
/// @file            TestCacheService.cpp
/// @authors         VLatsyn
/// @date            2/7/17
///
/// @brief           Test for CacheService
///
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <memory>
#include <utility>

#include <CL/Logger.hpp>
#include <CL/CacheService/ICacheService.hpp>
#include <CL/CacheService/Types.hpp>

namespace Bluetooth { namespace CL {

void _InitLogger() {
  SET_LOGGING_INFORMATION_SETTINGS("BTPlatform", "bluetooth_launcher");
  CL::Logging::Logger::SetFilter("");
}

void ready(tID ID, eImageSize size,tWeakIDataPtr image) {
  std::cout<<"==================\n";
  std::cout<<"ID "<<int(ID)<<" is ready\n";
  std::cout<<"size ";

  if (size==eImageSize::BIG) {
    std::cout<<"big";
  }
  else {
    std::cout<<"small";
  }

  std::cout<<"\n";
  std::cout<<"week_ptr<Data> ";

  if(image.lock()) {
    std::cout<<"NOT NULL\n";
  }
  else {
    std::cout<<"NULL\n";
  }

  std::cout<<"==================\n";
};

struct DataProvider final
  : IDataProvider
{
    virtual tPairOfIDataPtr Get(tID _id) override final {
      ///Patch -> Get pointers for images for ID
      tIDataPtr big;
      tIDataPtr small;
      ///End patch
      return std::make_pair(big,small);
    }
};
  
}} /// end namespace Bluetooth::CL


int main (){
  using namespace Bluetooth::CL;
  _InitLogger();

  tIDataProviderPtr provider (new DataProvider);
  tIDataProviderPtr provider2 (new DataProvider);

  auto mycache=GetCacheServiceInstance().AcquireCache(std::move(provider)).lock();
  auto mycache2=GetCacheServiceInstance().AcquireCache(std::move(provider2)).lock();
  boost::signals2::connection con=mycache->AttachtOnImageReadySignal(&ready);
  boost::signals2::connection con2=mycache2->AttachtOnImageReadySignal(&ready);

  mycache->GiveMeBig(1);
  mycache->GiveMeSmall(1);
  mycache2->GiveMeBig(4);
  mycache->GiveMeSmall(1);

  std::this_thread::sleep_for(std::chrono::seconds(2));//Need to see all signals

  GetCacheServiceInstance().ReleaseCache(std::move(mycache));
  GetCacheServiceInstance().ReleaseCache(std::move(mycache2));

  return 0;
}
