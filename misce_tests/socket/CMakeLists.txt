cmake_minimum_required(VERSION 3.0.0)
project(socket)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wno-unused-parameter -Wno-c++11-extensions")
add_executable(chat_server
    chat_server.cpp
)

add_executable(chat_client
    chat_client.cpp
)
