#include "AndroidVideoCapture.h"

void processVideo(const std::string& path)
{
    FFmpegVideoCapture cap;
    if (!cap.open(path.c_str()))
    {
        fprintf(stderr, "Cannot open video: %s\n", path.c_str());
        return;
    }

    cv::Mat frame;
    int frame_id = 0;
    while (cap.read(frame))
    {
        // 使用 OpenCV 显示或处理 BGR 帧
        cv::imwrite("imgs/frame" + std::to_string(frame_id) + ".jpg", frame);
        std::printf("Processed frame: %d\n", frame_id);
        frame_id++;
    }

    cap.release();
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <video_path>\n", argv[0]);
        return -1;
    }
    std::string videoPath = argv[1];
    processVideo(videoPath);
    return 0;
}