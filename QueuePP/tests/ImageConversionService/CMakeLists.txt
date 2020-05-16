
set(SOURCE_CACHE_DIR ../../source/CL/ImageConversionService)
set(HEADERS_CACHE_DIR ../../include/CL/ImageConversionService)
include_directories(../../include)
include_directories(../../include/CL)
include_directories(../../source/CL)
include_directories(../../source/CL/ImageConversionService)
include_directories(../../tests/ImageConversionService)

set(HEADERS
  #[[  ${HEADERS_CACHE_DIR}/ICacheService.hpp
    ${HEADERS_CACHE_DIR}/CacheService.hpp
    ${HEADERS_CACHE_DIR}/IDataProvider.hpp
    ${HEADERS_CACHE_DIR}/IData.hpp
    ${HEADERS_CACHE_DIR}/Data.hpp
    ${HEADERS_CACHE_DIR}/Types.hpp
    ${HEADERS_CACHE_DIR}/ICache.hpp
    ${HEADERS_CACHE_DIR}/Cache.hpp]]
  )

add_library(ImageConversionServiceForTest
  ${HEADERS}
  ${SOURCE_CACHE_DIR}/BaseConvertor.cpp
  ${SOURCE_CACHE_DIR}/BmpConvertor.cpp
  ${SOURCE_CACHE_DIR}/IConvertor.cpp
  ${SOURCE_CACHE_DIR}/IImageConversionService.cpp
  ${SOURCE_CACHE_DIR}/ImageConversionService.cpp
  ${SOURCE_CACHE_DIR}/JpgConvertor.cpp
  ${SOURCE_CACHE_DIR}/GIF.cpp
  ${SOURCE_CACHE_DIR}../../../CL/Data/Data.cpp
  )

target_link_libraries(ImageConversionServiceForTest
  ${Boost_LIBRARIES}
  ${CL_LIBRARIES}
  CL
  png
  jpeg
  gif
  )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable( TestImageConversionService
  main.cpp
  Test.cpp
  )

#add_executable( GTestImageConversionService
#  GTest.cpp
#  ../../source/CL/CacheService/Data.cpp
#  )


target_link_libraries(TestImageConversionService
  ImageConversionServiceForTest
  pthread
  ${CL_LIBRARIES}
  CL
  Logging
  )