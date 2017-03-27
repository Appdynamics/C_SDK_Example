cmake_minimum_required(VERSION 3.6)
project(C_SDK_Example)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/main/cpp/C_SDK_example.c
    README.md)

set(APPD_INCLUDE "extlibs/appdynamics-sdk-native/sdk_lib")

include_directories(${APPD_INCLUDE})

link_directories(extlibs/appdynamics-sdk-native/sdk_lib/lib)

add_executable(C_SDK_Example ${SOURCE_FILES})

target_link_libraries(C_SDK_Example appdynamics_native_sdk)

