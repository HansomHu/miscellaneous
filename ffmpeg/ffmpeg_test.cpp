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
const std::string rtmpUrl = "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8";

int main(int argc, char** argv) {
    std::cout << avcodec_configuration() << std::endl;
    std::cout << "Working directory: " << getcwd(nullptr, 0) << std::endl;
    av_register_all();
    AVFormatContext* fmtCtx = nullptr;
#if 0
    // Open network media
    avformat_network_init(); // Don't forget avformat_network_deinit() later
    AVDictionary* opt = nullptr;
    av_dict_set(&opt, "rtsp_transport", "tcp", 0);
    av_dict_set(&opt, "max_delay", "550", 0);
    auto ret = avformat_open_input(&fmtCtx, rtmpUrl.c_str(), nullptr, &opt);
#else
    // Open local media
    auto ret = avformat_open_input(&fmtCtx, videoPath.c_str(), nullptr, nullptr);
#endif
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