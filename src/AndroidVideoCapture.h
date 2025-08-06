#ifndef ANDROID_VIDEO_CAPTURE_H
#define ANDROID_VIDEO_CAPTURE_H
#include <opencv2/opencv.hpp>
#include <string>

class FFmpegVideoCapture
{
public:
    FFmpegVideoCapture();
    ~FFmpegVideoCapture();

    bool open(const std::string& filename);
    bool isOpened() const;
    bool read(cv::Mat& frame, int des_width=1920, int des_height=1080);
    void release();

    double getFPS() const;
    int getWidth() const;
    int getHeight() const;
    int getFrameCount() const;
    int getBitRate() const;

private:
    struct Impl;
    Impl* impl_;
};

#endif // ANDROID_VIDEO_CAPTURE_H