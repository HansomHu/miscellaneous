find_package(FFmpeg REQUIRED)

include_directories(${FFMPEG_INCLUDE_DIRS})

add_executable(ffmpeg_test ffmpeg_test.cpp)

target_link_libraries(ffmpeg_test ${FFMPEG_LIBRARIES})
message(STATUS ${FFMPEG_LIBRARIES})