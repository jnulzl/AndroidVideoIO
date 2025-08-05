extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <stdexcept>

#include "AndroidVideoCapture.h"
#include "android_videoio_version.h"

struct FFmpegVideoCapture::Impl
{
    AVFormatContext* fmt_ctx = nullptr;
    AVCodecContext* codec_ctx = nullptr;
    const struct AVCodec* decoder = nullptr;
    AVStream* video_stream = nullptr;
    int video_stream_index = -1;

    AVFrame* frame = nullptr;
    AVPacket* pkt = nullptr;
    SwsContext* sws_ctx = nullptr;

    int width = 0;
    int height = 0;
    int bit_rate = 0;
    AVRational time_base{};
    double fps = 0.0;
    int frame_count = 0;

    bool open(const std::string& filename)
    {
        avformat_network_init();

        if (avformat_open_input(&fmt_ctx, filename.c_str(), nullptr, nullptr) < 0)
        {
            return false;
        }

        if (avformat_find_stream_info(fmt_ctx, nullptr) < 0)
        {
            return false;
        }

        video_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
        if (video_stream_index < 0)
        {
            return false;
        }

        video_stream = fmt_ctx->streams[video_stream_index];
        time_base = video_stream->time_base;

        codec_ctx = avcodec_alloc_context3(decoder);
        avcodec_parameters_to_context(codec_ctx, video_stream->codecpar);

        if (avcodec_open2(codec_ctx, decoder, nullptr) < 0)
        {
            return false;
        }

        width = codec_ctx->width;
        height = codec_ctx->height;
        bit_rate = codec_ctx->bit_rate;
        fps = av_q2d(video_stream->r_frame_rate);
        frame_count = video_stream->nb_frames;        

        frame = av_frame_alloc();
        pkt = av_packet_alloc();

        return true;
    }

    bool read(cv::Mat& out_mat)
    {
        while (av_read_frame(fmt_ctx, pkt) >= 0)
        {
            if (pkt->stream_index != video_stream_index)
            {
                av_packet_unref(pkt);
                continue;
            }

            if (avcodec_send_packet(codec_ctx, pkt) < 0)
            {
                av_packet_unref(pkt);
                return false;
            }

            av_packet_unref(pkt);

            if (avcodec_receive_frame(codec_ctx, frame) == 0)
            {
                // Convert to BGR
                if (!sws_ctx)
                {
                    sws_ctx = sws_getContext(width, height, codec_ctx->pix_fmt,
                        width, height, AV_PIX_FMT_BGR24,
                        SWS_BILINEAR, nullptr, nullptr, nullptr);
                }

                out_mat.create(height, width, CV_8UC3);
                uint8_t* dst_data[1] = { out_mat.data };
                int dst_linesize[1] = { static_cast<int>(out_mat.step) };

                sws_scale(sws_ctx, frame->data, frame->linesize, 0, height, dst_data, dst_linesize);
                return true;
            }
        }
        return false;
    }

    void release()
    {
        if (pkt)
        {
            av_packet_free(&pkt);
            pkt = nullptr;
        }
        if (frame)
        {
            av_frame_free(&frame);
            frame = nullptr;
        }
        if (sws_ctx)
        {
            sws_freeContext(sws_ctx);
            sws_ctx = nullptr;
        }
        if (codec_ctx)
        {
            avcodec_free_context(&codec_ctx);
            codec_ctx = nullptr;
        }
        if (fmt_ctx)
        {
            avformat_close_input(&fmt_ctx);
            fmt_ctx = nullptr;
        }
    }

    ~Impl()
    {
        release();
    }
};

FFmpegVideoCapture::FFmpegVideoCapture()
{
    impl_ = new Impl();
}

FFmpegVideoCapture::~FFmpegVideoCapture()
{
    delete impl_;
}

bool FFmpegVideoCapture::open(const std::string& filename)
{
    return impl_->open(filename);
}

bool FFmpegVideoCapture::isOpened() const
{
    return impl_->fmt_ctx != nullptr;
}

bool FFmpegVideoCapture::read(cv::Mat& frame)
{
    return impl_->read(frame);
}

void FFmpegVideoCapture::release()
{
    impl_->release();
}

double FFmpegVideoCapture::getFPS() const
{
    return impl_->fps;
}

int FFmpegVideoCapture::getWidth() const
{
    return impl_->width;
}

int FFmpegVideoCapture::getHeight() const
{
    return impl_->height;
}

int FFmpegVideoCapture::getFrameCount() const
{
    return impl_->frame_count;
}

int FFmpegVideoCapture::getBitRate() const
{
    return impl_->bit_rate;
}