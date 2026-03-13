#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <vector> // NEW: We need this to hold our list of targets

using namespace cv;
using namespace std;

// --- NEW: TARGET DATA STRUCTURE ---
// This lets us create as many targets as we want with their own physical locations
struct RadarTarget {
    double offsetX; // Meters Left/Right from the drop center
    double offsetY; // Meters Up/Down from the drop center
    double size;    // Physical width in meters
};

int main() {
    // --- DIGITAL SCREEN SETTINGS ---
    int windowSize = 800; 
    double fps = 30.0;    
    double timeStep = 1.0 / fps; 
    
    // --- RADAR HEIGHT CALC ---
    double height = 200.0;       
    double velocity = 13.0;      
    
    // --- RADAR VARIABLES ---
    double fovDegrees = 25.0;      
    double tiltDegrees = 10.0;     
    double rps = 3.0; // 3 Revolutions Per Second
    
    // --- NEW: SETUP MULTIPLE TARGETS ---
    // We create a list (vector) of targets on the ground
    vector<RadarTarget> targets = {
        {0.0, 0.0, 2.0},    // Target 1: Dead center, 3m wide
        {15.0, -10.0, 2.0}, // Target 2: 15m Right, 10m Up, 4m wide
        {-12.0, 18.0, 2.0}, // Target 3: 12m Left, 18m Down, 2m wide
        {25.0, 25.0, 2.0}   // Target 4: Way out in the corner, 5m wide
    };
    
    double pi = 3.14159265;
    double fovRadians = fovDegrees * pi / 180.0;
    double tiltRadians = tiltDegrees * pi / 180.0;
    
    int frameCount = 0;
    RNG rng(12345); // Random Number Generator for noise

    cout << "Starting Case 4 (Multiple Scattered Targets)..." << endl;

    // --- MAIN LOOP ---
    while (height > 0.0) {
        Mat frame(windowSize, windowSize, CV_8UC3, Scalar(20, 20, 20));

        // -- 1. WHAT THE RADAR SEES (FOV) --
        double groundWidthMeters = 2.0 * height * tan(fovRadians);
        
        // -- 2. DIGITAL SCREEN SCALING --
        double pixelsPerMeter = windowSize / groundWidthMeters;

        // -- 3. UNDERSTANDING THE TILT & ROTATION --
        double shiftMeters = height * tan(tiltRadians);
        int shiftPixels = shiftMeters * pixelsPerMeter;
        double currentAngleRadians = (frameCount * timeStep) * (rps * 2.0 * pi);

        int centerX = windowSize / 2;
        int centerY = windowSize / 2;

        int tiltShiftX = shiftPixels * cos(currentAngleRadians);
        int tiltShiftY = shiftPixels * sin(currentAngleRadians);

        // -- 4. LOOP THROUGH AND DRAW EVERY TARGET --
        for (int t = 0; t < targets.size(); t++) {
            
            // Get the physical size in pixels for THIS specific target
            int targetPixels = targets[t].size * pixelsPerMeter;
            
            // Convert its physical ground offset into pixel offset
            int offsetPixelX = targets[t].offsetX * pixelsPerMeter;
            int offsetPixelY = targets[t].offsetY * pixelsPerMeter;

            // Calculate the exact center of this specific target on the screen
            int exactTargetCenterX = centerX + tiltShiftX + offsetPixelX;
            int exactTargetCenterY = centerY + tiltShiftY + offsetPixelY;

            // Draw the noise scatter for THIS target
            int numScatterPoints = targetPixels * 20; 

            for (int i = 0; i < numScatterPoints; i++) {
                int scatterX = exactTargetCenterX + rng.gaussian(targetPixels / 3.0);
                int scatterY = exactTargetCenterY + rng.gaussian(targetPixels / 3.0);

                // Only draw if the pixel is actually visible on the screen
                if (scatterX >= 0 && scatterX < windowSize && scatterY >= 0 && scatterY < windowSize) {
                    int greenIntensity = 150 + (rng.uniform(0, 105)); 
                    circle(frame, Point(scatterX, scatterY), 1, Scalar(0, greenIntensity, 0), FILLED);
                }
            }
        } // End of target loop

        // -- SAVE FRAME --
        string filename = "C:\\CPP\\radar\\case5\\case5a\\frames\\frame_" + to_string(frameCount) + ".png";
        imwrite(filename, frame);

        // -- UPDATE PHYSICS --
        height = height - (velocity * timeStep);
        frameCount++;
        
        if (frameCount % 30 == 0) {
            cout << "Saved 1 second of video... Height is now: " << height << "m" << endl;
        }
    }

    cout << "Finished! Total frames saved: " << frameCount << endl;

    return 0;
}