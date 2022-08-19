# FFmpeg
find_package(FFmpeg REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(
    ${FFMPEG_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(ffmpeg_test ffmpeg_test.cpp)

target_link_libraries(ffmpeg_test
    ${FFMPEG_LIBRARIES}
    ${OpenCV_LIBRARIES}
)
add_executable(ffmpeg_encoder ffmpeg_encoder.cpp)

target_link_libraries(ffmpeg_encoder
    ${FFMPEG_LIBRARIES}
    ${OpenCV_LIBRARIES}
)
message(STATUS ${FFMPEG_LIBRARIES})

