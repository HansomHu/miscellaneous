cmake_minimum_required(VERSION 3.0.0)
project(miscellaneous VERSION 0.1.0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wno-unused-parameter -O3")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
add_executable(miscellaneous main.cpp)

target_include_directories(miscellaneous PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(struct_and_algo)
add_subdirectory(ffmpeg)
add_subdirectory(misce_tests/OpenCV)
