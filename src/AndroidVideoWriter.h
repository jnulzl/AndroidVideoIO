#ifndef ANDROID_VIDEO_WRITER_H
#define ANDROID_VIDEO_WRITER_H
#include <opencv2/opencv.hpp>
#include <string>

class FFmpegVideoWriter {
public:
    FFmpegVideoWriter();
    ~FFmpegVideoWriter();

    bool open(const std::string& filename, int width, int height, double fps, int bit_rate = 4000000);
    bool isOpened() const;
    void write(const cv::Mat& frame);
    void release();

private:
    struct Impl;
    Impl* impl_;
};
#endif // ANDROID_VIDEO_WRITER_H
