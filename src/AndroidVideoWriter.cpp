#include "AndroidVideoWriter.h"
#include "android_videoio_version.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <iostream>

struct FFmpegVideoWriter::Impl
{
    AVFormatContext* fmt_ctx = nullptr;
    AVStream* video_stream = nullptr;
    AVCodecContext* codec_ctx = nullptr;
    SwsContext* sws_ctx = nullptr;
    AVFrame* frame = nullptr;
    AVPacket* pkt = nullptr;
    int frame_index = 0;
    int width = 0;
    int height = 0;
    AVRational time_base;

    bool open(const std::string& filename, int w, int h, double fps, int bit_rate)
    {
        avformat_alloc_output_context2(&fmt_ctx, nullptr, nullptr, filename.c_str());
        if (!fmt_ctx)
        {
            return false;
        }

        const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        if (!codec)
        {
            return false;
        }

        video_stream = avformat_new_stream(fmt_ctx, codec);
        if (!video_stream)
        {
            return false;
        }

        codec_ctx = avcodec_alloc_context3(codec);
        width = w;
        height = h;

        codec_ctx->codec_id = AV_CODEC_ID_H264;
        codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
        codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
        codec_ctx->width = width;
        codec_ctx->height = height;
        codec_ctx->time_base = AVRational{ 1, static_cast<int>(fps) };
        codec_ctx->framerate = AVRational{ static_cast<int>(fps), 1 };
        codec_ctx->gop_size = fps;
        codec_ctx->max_b_frames = 0;
        codec_ctx->bit_rate = bit_rate;

        if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        {
            codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        if (avcodec_open2(codec_ctx, codec, nullptr) < 0)
        {
            return false;
        }

        avcodec_parameters_from_context(video_stream->codecpar, codec_ctx);
        video_stream->time_base = codec_ctx->time_base;
        time_base = codec_ctx->time_base;

        if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE))
        {
            if (avio_open(&fmt_ctx->pb, filename.c_str(), AVIO_FLAG_WRITE) < 0)
            {
                return false;
            }
        }

        if (avformat_write_header(fmt_ctx, nullptr) < 0)
        {
            return false;
        }

        frame = av_frame_alloc();
        pkt = av_packet_alloc();

        frame->format = codec_ctx->pix_fmt;
        frame->width = width;
        frame->height = height;
        av_frame_get_buffer(frame, 0);

        sws_ctx = sws_getContext(width, height, AV_PIX_FMT_BGR24,
            width, height, AV_PIX_FMT_YUV420P,
            SWS_BICUBIC, nullptr, nullptr, nullptr);

        return true;
    }

    void write(const cv::Mat& bgr)
    {
        if (!bgr.data || bgr.cols != width || bgr.rows != height)
        {
            return;
        }

        uint8_t* src_data[1] = { (uint8_t*)bgr.data };
        int src_stride[1] = { static_cast<int>(bgr.step) };

        av_frame_make_writable(frame);

        sws_scale(sws_ctx, src_data, src_stride, 0, height,
            frame->data, frame->linesize);

        frame->pts = frame_index++;

        if (avcodec_send_frame(codec_ctx, frame) < 0)
        {
            return;
        }

        while (avcodec_receive_packet(codec_ctx, pkt) == 0)
        {
            pkt->stream_index = video_stream->index;
            pkt->pts = av_rescale_q(pkt->pts, codec_ctx->time_base, video_stream->time_base);
            pkt->dts = av_rescale_q(pkt->dts, codec_ctx->time_base, video_stream->time_base);
            pkt->duration = av_rescale_q(pkt->duration, codec_ctx->time_base, video_stream->time_base);

            av_interleaved_write_frame(fmt_ctx, pkt);
            av_packet_unref(pkt);
        }
    }

    void release()
    {
        if (codec_ctx)
        {
            avcodec_send_frame(codec_ctx, nullptr); // Flush
        }

        while (codec_ctx && avcodec_receive_packet(codec_ctx, pkt) == 0)
        {
            pkt->stream_index = video_stream->index;
            pkt->pts = av_rescale_q(pkt->pts, codec_ctx->time_base, video_stream->time_base);
            pkt->dts = av_rescale_q(pkt->dts, codec_ctx->time_base, video_stream->time_base);
            pkt->duration = av_rescale_q(pkt->duration, codec_ctx->time_base, video_stream->time_base);
            av_interleaved_write_frame(fmt_ctx, pkt);
            av_packet_unref(pkt);
        }

        if (fmt_ctx)
        {
            av_write_trailer(fmt_ctx);            
        }
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
            if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE))
            {
                avio_closep(&fmt_ctx->pb);
            }
            avformat_free_context(fmt_ctx);
            fmt_ctx = nullptr;
        }
    }

    ~Impl()
    {
        release();
    }
};

FFmpegVideoWriter::FFmpegVideoWriter()
{
    impl_ = new Impl();
}

FFmpegVideoWriter::~FFmpegVideoWriter()
{
    delete impl_;
}

bool FFmpegVideoWriter::open(const std::string& filename, int width, int height, double fps, int bit_rate)
{
    return impl_->open(filename, width, height, fps, bit_rate);
}

bool FFmpegVideoWriter::isOpened() const
{
    return impl_->fmt_ctx != nullptr;
}

void FFmpegVideoWriter::write(const cv::Mat& frame)
{
    impl_->write(frame);
}

void FFmpegVideoWriter::release()
{
    impl_->release();
}