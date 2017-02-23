cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}")

find_package(ZLIB REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)

set(OPENNI_ROOT /home/chao/Downloads/OpenNI2)
set(OPENNI_INCLUDE_DIR ${OPENNI_ROOT}/Include)
#set(OpenNI_LIBRARIES ${OPENNI_ROOT}/Bin/x64-Release/)

find_library(OpenNI_LIBRARIES OpenNI2 ${OPENNI_ROOT}/Bin/x64-Release/)

INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OPENNI_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(../OpenNI2)

aux_source_directory(. DIR_SRCS)

add_executable(RGBDCapture ${DIR_SRCS})

target_link_libraries(RGBDCapture
                      ${ZLIB_LIBRARY}
                      ${OpenNI_LIBRARIES}
                      ${OpenCV_LIBS}
		      ${Boost_SYSTEM_LIBRARIES}
                      ${Boost_FILESYSTEM_LIBRARIES}
                      )


# use C++11
target_compile_features(RGBDCapture PRIVATE cxx_range_for)


