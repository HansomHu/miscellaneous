# FFmpeg
if(APPLE)
    # ${FFMPEG_INCLUDE_DIRS} & ${FFMPEG_LIBRARIES} will be found
    find_package(FFmpeg REQUIRED)
elseif(UNIX)
    # on Ubuntu, FFmpeg should be installed from source code and the common install paths seem
    # like the following
    find_path(FFMPEG_INCLUDE_DIRS libavcodec/avcodec.h HINTS /usr/local/ffmpeg/include)
    find_path(FFMPEG_LIBRARY_DIRS libavcodec.so HINTS /usr/local/ffmpeg/lib)
    if(NOT FFMPEG_INCLUDE_DIRS OR NOT FFMPEG_LIBRARY_DIRS)
        message(FATAL "FFMPEG is not found, please check your system!")
    endif()

    foreach(lib IN ITEMS avdevice avfilter avcodec avformat swresample swscale avutil)
        set(LIB "LIB-NOTFOUND")
        find_library(LIB NAME ${lib} HINTS ${FFMPEG_LIBRARY_DIRS})
        list(APPEND FFMPEG_LIBRARIES ${LIB})
    endforeach()
endif()

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

