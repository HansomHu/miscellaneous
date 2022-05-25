cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)

project(test_so)

add_library(myfunc SHARED
    myfunc.cpp
)

target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBRARIES}
    gflags
)

