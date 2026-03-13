#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <cmath> // Needed for sin() and cos()

using namespace cv;
using namespace std;

int main() {
    // --- 1. Display & Video Settings ---
    int windowSize = 800; 
    double fps = 30.0;    
    
    // --- 2. Flight Variables ---
    double height = 200.0;       
    double velocity = 13.0;      
    double rps = 3.0; // 3 Rotations Per Second
    
    double timeStep = 1.0 / fps; 
    
    // --- 3. Radar Variables ---
    double fovDegrees = 25.0;
    double tiltDegrees = 10.0;
    double targetSizeMeters = 3.0; 
    
    // physical offset for the target
    double targetOffsetX = 150.0; // 20 meters to the Right of the drop zone
    double targetOffsetY = 0.0; // 15 meters Down from the drop zone
    
    double pi = 3.14159265;
    double fovRadians = fovDegrees * pi / 180.0;
    double tiltRadians = tiltDegrees * pi / 180.0;
    
    int frameCount = 0;
    double simulationTime = 0.0;

    cout << "Generating high-quality 30 FPS spinning frames with offset..." << endl;

    // --- 4. Main Simulation Loop ---
    while (height > 0.0) {
        Mat frame(windowSize, windowSize, CV_8UC3, Scalar(30, 30, 30));

        // Scale physical meters to screen pixels
        double groundWidthMeters = 2.0 * height * tan(fovRadians);
        double pixelsPerMeter = windowSize / groundWidthMeters;

        // Calculate size and tilt-shift in pixels
        double targetPixels = targetSizeMeters * pixelsPerMeter;
        double shiftMeters = height * tan(tiltRadians);
        double shiftPixels = shiftMeters * pixelsPerMeter;

        // NEW: Convert the target's physical offset into screen pixels
        double offsetPixelsX = targetOffsetX * pixelsPerMeter;
        double offsetPixelsY = targetOffsetY * pixelsPerMeter;

        // --- THE SPIN MATH ---
        double spinAngle = rps * 2.0 * pi * simulationTime;

        int centerX = windowSize / 2;
        int centerY = windowSize / 2;

        // Effect 1: The Orbit + The Physical Offset
        // We add the offsetPixels so the target is pushed away from the dead center
        double targetCenterX = centerX + offsetPixelsX + (shiftPixels * cos(spinAngle));
        double targetCenterY = centerY + offsetPixelsY + (shiftPixels * sin(spinAngle));

        // Effect 2: The Box Spin
        double halfBox = targetPixels / 2.0;
        double boxX[4] = {-halfBox,  halfBox,  halfBox, -halfBox};
        double boxY[4] = {-halfBox, -halfBox,  halfBox,  halfBox};

        Point pixelCorners[4];

        // Loop through the 4 corners and rotate them
        for(int i = 0; i < 4; i++) {
            double rotatedX = (boxX[i] * cos(spinAngle)) - (boxY[i] * sin(spinAngle));
            double rotatedY = (boxX[i] * sin(spinAngle)) + (boxY[i] * cos(spinAngle));

            int finalX = (int)(targetCenterX + rotatedX);
            int finalY = (int)(targetCenterY + rotatedY);

            pixelCorners[i] = Point(finalX, finalY);
        }

        // Draw the spinning, orbiting target
        fillConvexPoly(frame, pixelCorners, 4, Scalar(0, 255, 0));

        // -- On-Screen Text --
        string timeText = "Time: " + to_string((int)simulationTime) + " s";
        string heightText = "Height: " + to_string((int)height) + " m";
        
        putText(frame, timeText, Point(20, 40), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 255), 2);
        putText(frame, heightText, Point(20, 80), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 255), 2);

        // ---------------- SAVE FRAME ----------------
        string filename = "C:\\CPP\\radar\\case3\\case3d\\frames\\frame_" + to_string(frameCount) + ".png";
        imwrite(filename, frame);

        // Update physics for the next frame
        height = height - (velocity * timeStep);
        simulationTime = simulationTime + timeStep;
        frameCount++;
        
        if (frameCount % 30 == 0) {
            cout << "Processed 1 second of video (" << frameCount << " frames)... Height is: " << height << "m" << endl;
        }
    }

    cout << "Done! Total Frames: " << frameCount << endl;

    return 0;
}