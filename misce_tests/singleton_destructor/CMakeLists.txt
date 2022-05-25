cmake_minimum_required(VERSION 3.0.0)
project(singleton)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wno-unused-parameter -Wno-c++11-extensions")
add_executable(${PROJECT_NAME}
    singleton.cpp
)
