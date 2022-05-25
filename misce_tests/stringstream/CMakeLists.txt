cmake_minimum_required(VERSION 3.0.0)
project(stringstream)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wno-unused-parameter")
add_executable(${PROJECT_NAME}
    stringstream.cpp
)
