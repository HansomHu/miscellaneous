#include <unistd.h> // getcwd()
#include <iostream>
#include <string>
#include <thread>

#include <opencv2/opencv.hpp>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
}
#endif

// ffmpeg的API是否为新版本
#define NEW_API_VERSION 1

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
    av_dict_free(&opt);
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

    AVCodecContext* codecCtx = nullptr;
    int videoIndex = -1;

#if NEW_API_VERSION == 0
    // NOTE: fmCtx->streams[index]->codec字段已经被废弃(codecpar代替)，官方不建议使用该方法获得codecCtx，当然现阶段这样写也没问题
    // 判断媒体文件中的流类型，并获取video流的index
    for (int index = 0; index < fmtCtx->nb_streams; index++) {
        codecCtx = fmtCtx->streams[index]->codec;
        videoIndex = codecCtx->codec_type == AVMEDIA_TYPE_VIDEO ? index : videoIndex;
        std::cout << "流序号: " << index << ", 类型为: " << av_get_media_type_string(codecCtx->codec_type) << std::endl;
        // 已经找打视频品流
        if (videoIndex != -1) {
            break;
        }
    }

    // 查找指定的已注册过的解码器, 不需要手动释放decoder指针
    auto decoder = avcodec_find_decoder(codecCtx->codec_id);
#else
    // NOTE: 这段获取codecCtx代码是4.0版本（或者更老？）官方example里的用法
    videoIndex = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (videoIndex < 0) {
        std::cout << "Could not find " << av_get_media_type_string(AVMEDIA_TYPE_VIDEO)
            << " stream in input file: " << videoPath << std::endl;
        return -1;
    }
    auto stream = fmtCtx->streams[videoIndex];
    // 查找指定的已注册过的解码器, 不需要手动释放decoder指针
    auto decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    // 使用avcodec_alloc_context3()来手动申请codecCtx结构体内存，最后需要手动释放内存
    codecCtx = avcodec_alloc_context3(decoder);
    if (!codecCtx) {
        std::cout << "Failed to allocate the " << av_get_media_type_string(AVMEDIA_TYPE_VIDEO) << " codec context\n";
        return -1;
    }
    // Copy codec parameters from input stream to output codec context
    if (avcodec_parameters_to_context(codecCtx, stream->codecpar) < 0) {
        std::cout << "Failed to copy " << av_get_media_type_string(AVMEDIA_TYPE_VIDEO) << " codec parameters to decoder context\n";
        return -1;
    }
#endif

    if (auto ret = avcodec_open2(codecCtx, decoder, nullptr)) {
        std::cout << "打开解码器失败: " << ret << std::endl;
        return -2;
    }

    // 获得SwsContext上下文，可以用来对颜色空间、尺寸进行调整
    auto swsCtx = sws_getContext(codecCtx->width, codecCtx->height, codecCtx->pix_fmt,
        codecCtx->width, codecCtx->height, AV_PIX_FMT_BGRA, SWS_FAST_BILINEAR, 0, 0, 0);
    // 获得指定尺寸图像的内存大小
    auto numBytes = avpicture_get_size(AV_PIX_FMT_BGRA, codecCtx->width, codecCtx->height);
    // 用户分配空间，以存放解码后的图像数据
    auto outBuffer = std::shared_ptr<uint8_t>((uint8_t*)av_malloc(numBytes), [](auto ptr) {
        av_free(ptr);
    });
    auto avFrame = av_frame_alloc();
    auto avFrameRGB32 = av_frame_alloc();
    avpicture_fill((AVPicture*)avFrameRGB32, outBuffer.get(), AV_PIX_FMT_BGRA, codecCtx->width, codecCtx->height);

    // 开始读取数据帧并解码
    AVPacket* avPacket = av_packet_alloc();
    int packetNum = 0;
    int frameNum = 0;
    while (av_read_frame(fmtCtx, avPacket) >= 0) {
        if (avPacket->stream_index == videoIndex) {
            ++packetNum;
            // 发送数据给编码器
            if (auto ret = avcodec_send_packet(codecCtx, avPacket)) {
                std::cout << "Error: avcodec_send_packet() failed: " << ret << std::endl;
                break;
            }
            // 一个packet中可能存在多个frame
            while (!avcodec_receive_frame(codecCtx, avFrame)) {
                ++frameNum;
                sws_scale(swsCtx, avFrame->data, avFrame->linesize, 0,
                    codecCtx->height, avFrameRGB32->data, avFrameRGB32->linesize);
                // cv::Mat mat(codecCtx->height, codecCtx->width, CV_8UC4, outBuffer.get());
                // std::cout << "==== write frame: " << frameNum << std::endl;
                // cv::imwrite("frame_" + std::to_string(frameNum) + ".jpg", mat);
            }
        }
        // 不要忘记unref packet
        av_packet_unref(avPacket);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "Total packets: " << packetNum << ", total frame: " << frameNum << std::endl;
    std::cout << "释放回收资源...\n";
    if (avPacket) {
        av_packet_free(&avPacket);
        std::cout << "av_packet_free(&avPacket)\n";
    }
    if (avFrameRGB32) {
        av_frame_free(&avFrameRGB32);
        std::cout << "av_frame_free(&avFrameRGB32)\n";
    }
    if (avFrame) {
        av_frame_free(&avFrame);
        std::cout << "av_frame_free(&avFrame)\n";
    }
    if (swsCtx) {
        sws_freeContext(swsCtx);
        std::cout << "sws_freeContext(swsCtx)\n";
    }
    if(codecCtx)
    {
        avcodec_close(codecCtx);
#if NEW_API_VERSION == 1
        avcodec_free_context(&codecCtx);
#endif
        codecCtx = 0;
        std::cout << "avcodec_close(codecCtx);\n";
    }
    if(fmtCtx)
    {
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        fmtCtx = 0;
        std::cout << "avformat_free_context(fmtCtx)";
    }
    return 0;
}