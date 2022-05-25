cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)

project(test_opencv)
find_package(OpenCV REQUIRED)
find_package(GFlags REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}    
    ${GFLAGS_INCLUDE_DIR}
)

add_executable(${PROJECT_NAME} test_opencv.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} gflags)

add_executable(traverse_triangle traverse_triangle.cpp)
target_link_libraries(traverse_triangle ${OpenCV_LIBRARIES})

add_executable(iou iou.cpp)
target_link_libraries(iou ${OpenCV_LIBRARIES})

message(STATUS "OpenCV libraries: ${OpenCV_LIBRARIES}")
message(STATUS "OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")
