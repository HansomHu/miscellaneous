#include <unistd.h> // getcwd()
#include <iostream>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
}
#endif

const std::string videoPath = "../../ffmpeg/test.mp4";
int main(int argc, char** argv) {
    std::cout << avcodec_configuration() << std::endl;
    std::cout << "Working directory: " << getcwd(nullptr, 0) << std::endl;
    av_register_all();
    AVFormatContext* fmtCtx = nullptr;
    auto ret = avformat_open_input(&fmtCtx, videoPath.c_str(), nullptr, nullptr);
    if (ret != 0) {
        char data[512];
        av_strerror(ret, data, 512);
        std::cout << "avformat_open_input() failed: " << ret << ", " << data << std::endl;
        return -1;
    }
    std::cout << "avformat_open_input() success!" << std::endl;
    if ((ret = avformat_find_stream_info(fmtCtx, nullptr)) < 0) {
        char data[512];
        av_strerror(ret, data, 512);
        std::cout << "avformat_find_stream_info() failed: " << ret << ", " << data << std::endl;
        return -1;
    }
    std::cout << "avformat_find_stream_info() success!" << std::endl;
    auto time = fmtCtx->duration;
    auto min = time / 1000000 / 60;
    auto sec = time / 1000000 - 60 * min;
    std::cout << "Total time: " << min << " m " << sec << " s" << std::endl;
    av_dump_format(fmtCtx, 0, videoPath.c_str(), 0); 

    return 0;
}