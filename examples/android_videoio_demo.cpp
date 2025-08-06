#include "AndroidVideoCapture.h"
#include "AndroidVideoWriter.h"

void videoDecode(const std::string& path)
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

void videoDecodeEncode(const std::string& path)
{
    FFmpegVideoCapture cap;
    if (!cap.open(path.c_str()))
    {
        fprintf(stderr, "Cannot open video: %s\n", path.c_str());
        return;
    }
    std::printf("Video FPS: %f, Width: %d, Height: %d\n", cap.getFPS(), cap.getWidth(), cap.getHeight());

    int des_width = 640;
    int des_height = 360;
    cv::Mat frame;
    int frame_id = 0;
    FFmpegVideoWriter writer;
    writer.open("imgs/output.mp4", des_width, des_height, cap.getFPS(), cap.getBitRate());
    if (!writer.isOpened())
    {
        fprintf(stderr, "Cannot open video writer.\n");
        return;
    }
    while (cap.read(frame, des_width, des_height))
    {
        // 使用 OpenCV 显示或处理 BGR 帧
        cv::imwrite("imgs/frame" + std::to_string(frame_id) + ".jpg", frame);
        std::printf("Processed frame: %d\n", frame_id);        
        writer.write(frame);
        frame_id++;
    }
    writer.release();
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
    videoDecodeEncode(videoPath);
    return 0;
}