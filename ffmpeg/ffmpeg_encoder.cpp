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

namespace helper {
static inline std::string avStrError(int errnum) {
    char buf[128] = {0};
    if (!av_strerror(errnum, buf, sizeof(buf))) {
        return buf;
    }
    return std::string("Unkown errnum: ") + std::to_string(errnum);
}
} // namespace helper

class EncoderH264 final {
public:
    EncoderH264(const std::string& filename) : m_outFile(filename) {}
    ~EncoderH264();
    bool addStream(int widht, int height);
    bool openOutputFile();
    bool openVideo();
    bool writeHeader();
    bool writeTrailer();
    bool encode(AVFrame* inputFrame);

protected:
    std::string m_outFile;
    AVFormatContext* m_fmtCtx = nullptr;
    AVCodecContext* m_codecCtx = nullptr;
    AVOutputFormat* m_outFmt = nullptr; // m_fmtCtx->oformat的备份
    AVCodec* m_codec = nullptr; // 不需要手动释放
    AVStream* m_stream = nullptr; // 不需要手动释放，归m_fmtCtx管理
    AVDictionary* m_opt = nullptr;
};

EncoderH264::~EncoderH264() {
    if (m_opt) {
        av_dict_free(&m_opt);
    }
    if (!(m_outFmt->flags & AVFMT_NOFILE)) {
        /* Close the output file. */
        avio_closep(&m_fmtCtx->pb);
    }
    if (m_codecCtx) {
        avcodec_close(m_codecCtx);
        avcodec_free_context(&m_codecCtx);
    }
    if (m_fmtCtx) {
        avformat_free_context(m_fmtCtx);
    }
}

bool EncoderH264::addStream(int widht, int height) {
    avformat_alloc_output_context2(&m_fmtCtx, NULL, NULL, m_outFile.c_str());
    if (!m_fmtCtx) {
        std::cout << "Could not deduce output format from file extension: " << m_outFile << std::endl;
        return false;
    }
    m_outFmt = m_fmtCtx->oformat;
    /* Add the audio and video streams using the default format codecs
     * and initialize the codecs. */
    if (m_outFmt->video_codec == AV_CODEC_ID_NONE) {
        std::cout << "No valid codec ID match the input file: " << m_outFile << std::endl;
        return false;
    }
    if (!(m_codec = avcodec_find_encoder(m_outFmt->video_codec))) {
        std::cout << "Could not find encoder for " << avcodec_get_name(m_outFmt->video_codec) << std::endl;
        return false;
    }
    if (!(m_stream = avformat_new_stream(m_fmtCtx, nullptr))) {
        std::cout << "Could not allocate stream\n";
        return false;
    }
    m_stream->id = m_fmtCtx->nb_streams - 1;
    if (!(m_codecCtx = avcodec_alloc_context3(m_codec))) {
        std::cout << "Could not allocate an encoding context\n";
        return false;
    }
    assert(m_codec->type == AVMEDIA_TYPE_VIDEO);
    m_codecCtx->codec_id = m_outFmt->video_codec;
    // m_codecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    m_codecCtx->bit_rate = 400000;
    m_codecCtx->width = widht;
    m_codecCtx->height = height;
    /* timebase: This is the fundamental unit of time (in seconds) in terms
    * of which frame timestamps are represented. For fixed-fps content,
    * timebase should be 1/framerate and timestamp increments should be
    * identical to 1. */
    m_stream->time_base = (AVRational){ 1, 25 };
    m_codecCtx->time_base = m_stream->time_base;

    m_codecCtx->gop_size = 12; /* emit one intra frame every twelve frames at most */
    m_codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    // H.264
    // m_codecCtx->me_range = 16;
    // m_codecCtx->max_qdiff = 4;
    // m_codecCtx->qcompress = 0.6;
    m_codecCtx->qmin = 10;
    m_codecCtx->qmax = 51;

    // Optional Param
    m_codecCtx->max_b_frames = 0;

    // H.264
    if (m_codecCtx->codec_id == AV_CODEC_ID_H264) {
        av_dict_set(&m_opt, "preset", "slow", 0);
        av_dict_set(&m_opt, "tune", "zerolatency", 0);
        // av_dict_set(&m_opt, "profile", "main", 0);
    }

    /* Some formats want stream headers to be separate. */
    if (m_fmtCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        m_codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    return true;
}

bool EncoderH264::openOutputFile() {
    /* open the output file, if needed */
    if (!(m_outFmt->flags & AVFMT_NOFILE)) {
        auto ret = avio_open(&m_fmtCtx->pb, m_outFile.c_str(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            std::cout << "Could not open " << m_outFile << helper::avStrError(ret) << std::endl;
            return false;
        }
        return true;
    }
    return false;
}

bool EncoderH264::openVideo() {
    int ret = -1;
    ret = avcodec_open2(m_codecCtx, m_codec, &m_opt);
    if (ret < 0) {
        std::cout << "Could not open video codec: " << helper::avStrError(ret) << std::endl;
        return false;
    }
    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(m_stream->codecpar, m_codecCtx);
    if (ret < 0) {
        std::cout << "Could not copy the stream parameters\n";
        return false;
    }
    av_dump_format(m_fmtCtx, 0, m_outFile.c_str(), 1);
    return true;
}

bool EncoderH264::writeHeader() {
    auto ret = avformat_write_header(m_fmtCtx, &m_opt);
    if (ret < 0) {
        std::cout << "Error occured when open file: " << helper::avStrError(ret) << std::endl;
        return false;
    }
    return true;
}

bool EncoderH264::writeTrailer() {
    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    av_write_trailer(m_fmtCtx);
    return true;
}

bool EncoderH264::encode(AVFrame* inputFrame) {
    auto ret = avcodec_send_frame(m_codecCtx, inputFrame);
    if (ret < 0) {
        std::cout << "Error sending a frame to encoder: " << helper::avStrError(ret) << std::endl;
        return false;
    }
    while (ret >= 0) {
        AVPacket pkt = { 0 };
        ret = avcodec_receive_packet(m_codecCtx, &pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            std::cout << "Error encoding a frame: " << helper::avStrError(ret) << std::endl;
            return false;
        }
        /* rescale output packet timestamp values from codec to stream timebase */
        av_packet_rescale_ts(&pkt, m_codecCtx->time_base, m_stream->time_base);
        pkt.stream_index = m_stream->index;
        ret = av_interleaved_write_frame(m_fmtCtx, &pkt);
        av_packet_unref(&pkt);
        if (ret < 0) {
            std::cout << "Error while writing output packet: " << helper::avStrError(ret) << std::endl;
            return false;
        }
    }
    return ret == AVERROR_EOF ? true : false;
}

const std::string videoPath = "../../ffmpeg/test.mp4";

int main(int argc, char** argv) {
    av_register_all();
    AVFormatContext* fmtCtx = nullptr;
    // Open local media
    auto ret = avformat_open_input(&fmtCtx, videoPath.c_str(), nullptr, nullptr);
    if (ret != 0) {
        std::cout << "avformat_open_input() failed: " << helper::avStrError(ret) << std::endl;
        return -1;
    }
    std::cout << "avformat_open_input() success!" << std::endl;
    if ((ret = avformat_find_stream_info(fmtCtx, nullptr)) < 0) {
        std::cout << "avformat_find_stream_info() failed: " << helper::avStrError(ret) << std::endl;
        return -1;
    }
    std::cout << "avformat_find_stream_info() success!" << std::endl;
    av_dump_format(fmtCtx, 0, videoPath.c_str(), 0); 

    AVCodecContext* codecCtx = nullptr;
    int videoIndex = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
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

    if (auto ret = avcodec_open2(codecCtx, decoder, nullptr)) {
        std::cout << "打开解码器失败: " << ret << std::endl;
        return -2;
    }

    // 获得SwsContext上下文，可以用来对颜色空间、尺寸进行调整
    auto swsCtx = sws_getContext(codecCtx->width, codecCtx->height, codecCtx->pix_fmt,
        codecCtx->width, codecCtx->height, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, 0, 0, 0);
    // 获得指定尺寸图像的内存大小
    auto numBytes = avpicture_get_size(AV_PIX_FMT_YUV420P, codecCtx->width, codecCtx->height);
    auto avFrame = av_frame_alloc();
    auto avFrameYUV420P = av_frame_alloc();
    avFrameYUV420P->format = AV_PIX_FMT_YUV420P;
    avFrameYUV420P->width = codecCtx->width;
    avFrameYUV420P->height = codecCtx->height;
    avFrameYUV420P->pts = 0;
    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(avFrameYUV420P, 0);

    // 开始读取数据帧并解码
    AVPacket* avPacket = av_packet_alloc();
    int packetNum = 0;
    int frameNum = 0;
    EncoderH264 encoder("test.h264");
    assert(encoder.addStream(codecCtx->width, codecCtx->height));
    assert(encoder.openVideo());
    assert(encoder.openOutputFile());
    assert(encoder.writeHeader());
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
                    codecCtx->height, avFrameYUV420P->data, avFrameYUV420P->linesize);
                avFrameYUV420P->pts++;
                encoder.encode(avFrameYUV420P);
            }
        }
        // 不要忘记unref packet
        av_packet_unref(avPacket);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    // TODO(hyx): 加上flush encoder代码，确保不丢帧

    assert(encoder.writeTrailer());

    std::cout << "Total packets: " << packetNum << ", total frame: " << frameNum << std::endl;
    std::cout << "释放回收资源...\n";
    if (avPacket) {
        av_packet_free(&avPacket);
        std::cout << "av_packet_free(&avPacket)\n";
    }
    if (avFrameYUV420P) {
        av_frame_free(&avFrameYUV420P);
        std::cout << "av_frame_free(&avFrameYUV420P)\n";
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
        avcodec_free_context(&codecCtx);
        codecCtx = 0;
        std::cout << "avcodec_close(codecCtx);\n";
    }
    if(fmtCtx)
    {
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        fmtCtx = 0;
        std::cout << "avformat_free_context(fmtCtx)\n";
    }
    return 0;
}
