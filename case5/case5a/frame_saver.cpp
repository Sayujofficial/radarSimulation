#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
    string folder = "C:/CPP/radar/case5/case5a/FRAMES/";
    string outputFile = "C:/CPP/radar/case5/case5a/output.avi";

    int fps = 30;

    Mat firstFrame = imread(folder + "frame_0.png");

    if (firstFrame.empty())
    {
        cout << "Could not read first image!" << endl;
        return -1;
    }

    Size frameSize(firstFrame.cols, firstFrame.rows);

    // MJPG codec (VERY stable)
    VideoWriter video(outputFile,
                      VideoWriter::fourcc('M','J','P','G'),
                      fps,
                      frameSize);

    if (!video.isOpened())
    {
        cout << "VideoWriter failed to open!" << endl;
        return -1;
    }

    int frameNumber = 0;

    while (true)
    {
        string filename = folder + "frame_" + to_string(frameNumber) + ".png";
        Mat frame = imread(filename);

        if (frame.empty())
            break;

        video.write(frame);
        frameNumber++;
    }

    video.release();

    cout << "Video created successfully!" << endl;
    cout << "Total frames used: " << frameNumber << endl;

    return 0;
}