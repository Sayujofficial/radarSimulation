#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <cmath>

using namespace cv;
using namespace std;

int main() {
    Scalar objectColor(0, 0, 255); // Red color for the target
    int windowSize = 800;
    double fps = 30.0;
    
    // --- Initial setup: Kitni height se gir raha hai aur speed kya hai ---
    double height = 200.0;
    double velocity = 13.0;
    double timeStep = 1.0 / fps; // Har frame ke beech ka gap
    
    double fovDegrees = 25.0;
    double tiltDegrees = -15.0;
    double targetSizeMeters = 3.0;
    
    double pi = 3.14159265;
    double fovRadians = fovDegrees * pi / 180.0;
    double tiltRadians = tiltDegrees * pi / 180.0;
    
    int frameCount = 0;
    double simulationTime = 0.0;

    cout << "Generating high-quality 30 FPS frames..." << endl;

    // --- Jab tak height 0 nahi hoti, loop chalega ---
    while (height > 0.0) {
        // Dark gray background create kar rahe hain
        Mat frame(windowSize, windowSize, CV_8UC3, Scalar(30, 30, 30));

        // Ground pe kitna area dikh raha hai (Physical calculation)
        double groundWidthMeters = 2.0 * height * tan(fovRadians);
        
        // Meters ko pixels mein convert karne ka logic (Scaling)
        double pixelsPerMeter = windowSize / groundWidthMeters;

        // Target ka size aur tilt ki wajah se shift calculate karo
        int targetPixels = (int)(targetSizeMeters * pixelsPerMeter);
        double shiftMeters = height * tan(tiltRadians);
        int shiftPixels = (int)(shiftMeters * pixelsPerMeter);

        int centerX = windowSize / 2;
        int centerY = windowSize / 2;

        // Screen pe target ki position (X is center, Y is tilted)
        int targetX = centerX - (targetPixels / 2);
        int targetY = centerY - (targetPixels / 2) - shiftPixels; 

        // Bright green rectangle draw kar rahe hain (objectColor use karke)
        Rect targetBox(targetX, targetY, targetPixels, targetPixels);
        rectangle(frame, targetBox, objectColor, FILLED);

        // Screen pe time aur height dikhane ke liye
        string timeText = "Time: " + to_string((int)simulationTime) + " s";
        string heightText = "Height: " + to_string((int)height) + " m";
        
        putText(frame, timeText, Point(20, 40), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 255), 2);
        putText(frame, heightText, Point(20, 80), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 255), 2);

        // Frame ko image file ki tarah save karo
        string filename = "C:\\CPP\\radar\\case2\\frames\\frame_" + to_string(frameCount) + ".png";
        imwrite(filename, frame);

        // Physics update: Height kam karo aur time badhao
        height = height - (velocity * timeStep);
        simulationTime = simulationTime + timeStep;
        frameCount++;
        
        if (frameCount % 30 == 0) {
            cout << "Processed 1 second of video (" << frameCount << " frames)..." << endl;
        }
    }

    cout << "Done! Total Frames: " << frameCount << endl;
    cout << "Total video length: " << simulationTime << " seconds." << endl;

    return 0;
}