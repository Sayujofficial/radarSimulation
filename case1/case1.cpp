#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
    int windowSize = 800;
    Mat image(windowSize, windowSize, CV_8UC3);

    float startHeight = 200.0;   // meters
    float velocity = 13.0;       // m/s
    float fov = 25.0;            // degrees
    float targetSize = 3.0;      // meters

    double startTime = (double)getTickCount();

    int frameCount = 0;

    while (true)
    {
        double currentTime = (double)getTickCount();
        double t = (currentTime - startTime) / getTickFrequency();

        float height = startHeight - velocity * t;

        if (height <= 0)
            break;

        image = Scalar(0,0,0);

        // FOOTPRINT
        float halfWidth = height * tan(fov * CV_PI / 180.0);
        float footprint = 2 * halfWidth;

        float scale = windowSize / footprint;

        int sizePixels = footprint * scale;
        int center = windowSize / 2;

        Rect radarSquare(center - sizePixels/2,
                         center - sizePixels/2,
                         sizePixels,
                         sizePixels);

        rectangle(image, radarSquare, Scalar(255,255,255), 2);

        int targetPixels = targetSize * scale;

        Rect targetSquare(center - targetPixels/2,
                          center - targetPixels/2,
                          targetPixels,
                          targetPixels);

        rectangle(image, targetSquare, Scalar(0,255,0), FILLED);

        string text = "Height: " + to_string((int)height) + " m";
        putText(image, text, Point(20,40),
                FONT_HERSHEY_SIMPLEX, 1,
                Scalar(0,255,255), 2);

        // SAVE FRAME (REAL-TIME)
      string filename = "C:\\CPP\\radar\\case1\\FRAMES\\frame_" + to_string(frameCount) + ".png";
        imwrite(filename, image);

        frameCount++;
    }

    cout << "Saved " << frameCount << " frames." << endl;

    return 0;
}