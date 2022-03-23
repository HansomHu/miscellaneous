cmake_minimum_required(VERSION 3.0.0)
project(miscellaneous VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wno-unused-parameter -O3")
add_executable(miscellaneous main.cpp)

target_include_directories(miscellaneous PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(struct_and_algo)