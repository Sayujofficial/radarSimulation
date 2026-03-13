#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

using namespace cv;
using namespace std;

// Blueprint for our multiple targets
struct RadarTarget {
    double offsetX; 
    double offsetY; 
    double size;    
};

int main() {
    // --- 1. DIGITAL SCREEN SETTINGS ---
    int windowSize = 800; 
    double fps = 30.0;    
    double timeStep = 1.0 / fps; 
    
    // --- 2. FLIGHT & RADAR VARIABLES ---
    double height = 200.0;       
    double dropVelocity = 13.0;      
    double fovDegrees = 25.0;      
    double tiltDegrees = 10.0;     
    double rps = 0.0; 
    
    // --- 3. NEW: WIND & OSCILLATION SETTINGS ---
    double windVelocityX = 3.0;  // Wind pushes Right at 3 m/s
    double windVelocityY = -1.5; // Wind pushes Up at 1.5 m/s

    double swingAmplitude = 8.0; // Max pendulum swing is 8 meters
    double swingFrequency = 0.2; // 0.2 swings per sec (1 full swing every 5 seconds)
    
    // Track where the radar actually is in the physical world
    double radarWorldX = 0.0;
    double radarWorldY = 0.0;

    // --- 4. THE TARGETS (Case 5) ---
    vector<RadarTarget> targets = {
        {0.0, 0.0, 3.0},    // Center target, 3m wide
        {15.0, -10.0, 4.0}, // Offset target, 4m wide
        {-12.0, 18.0, 2.0}, // Offset target, 2m wide
        {25.0, 25.0, 5.0}   // Far corner target, 5m wide
    };
    
    double pi = 3.14159265;
    double fovRadians = fovDegrees * pi / 180.0;
    double tiltRadians = tiltDegrees * pi / 180.0;
    
    int frameCount = 0;
    RNG rng(12345); // Random Number Generator for the noise scatter

    cout << "Starting Case 6 (Multiple Noisy Targets + Wind + Oscillation)..." << endl;

    // --- MAIN LOOP ---
    while (height > 0.0) {
        Mat frame(windowSize, windowSize, CV_8UC3, Scalar(20, 20, 20));
        double currentTime = frameCount * timeStep;

        // -- APPLY PHYSICS TO RADAR POSITION --
        radarWorldX = radarWorldX + (windVelocityX * timeStep);
        radarWorldY = radarWorldY + (windVelocityY * timeStep);

        double currentSwingX = swingAmplitude * sin(2.0 * pi * swingFrequency * currentTime);
        double currentSwingY = swingAmplitude * cos(2.0 * pi * swingFrequency * currentTime);

        // Radar's true position = Wind Drift + Pendulum Swing
        double totalRadarX = radarWorldX + currentSwingX;
        double totalRadarY = radarWorldY + currentSwingY;

        // -- CALCULATE FOV & SCALING --
        double groundWidthMeters = 2.0 * height * tan(fovRadians);
        double pixelsPerMeter = windowSize / groundWidthMeters;

        // -- TILT & ROTATION SHIFT --
        double shiftMeters = height * tan(tiltRadians);
        int shiftPixels = shiftMeters * pixelsPerMeter;
        double currentAngleRadians = currentTime * (rps * 2.0 * pi);

        int centerX = windowSize / 2;
        int centerY = windowSize / 2;

        int tiltShiftX = shiftPixels * cos(currentAngleRadians);
        int tiltShiftY = shiftPixels * sin(currentAngleRadians);

        // -- DRAW EVERY TARGET --
        for (int t = 0; t < targets.size(); t++) {
            
            int targetPixels = targets[t].size * pixelsPerMeter;
            
            // RELATIVE MOTION: Target Position minus Radar Position
            double distanceToTargetX = targets[t].offsetX - totalRadarX;
            double distanceToTargetY = targets[t].offsetY - totalRadarY;

            // Convert physical distance into screen pixels
            int offsetPixelX = distanceToTargetX * pixelsPerMeter;
            int offsetPixelY = distanceToTargetY * pixelsPerMeter;

            // Calculate exact center of this specific target on screen
            int exactTargetCenterX = centerX + tiltShiftX + offsetPixelX;
            int exactTargetCenterY = centerY + tiltShiftY + offsetPixelY;

            // Spray the noisy green dots (Gaussian Scatter)
            int numScatterPoints = targetPixels * 20; 

            for (int i = 0; i < numScatterPoints; i++) {
                int scatterX = exactTargetCenterX + rng.gaussian(targetPixels / 3.0);
                int scatterY = exactTargetCenterY + rng.gaussian(targetPixels / 3.0);

                // Only draw if the dot lands inside the 800x800 window
                if (scatterX >= 0 && scatterX < windowSize && scatterY >= 0 && scatterY < windowSize) {
                    int greenIntensity = 150 + (rng.uniform(0, 105)); 
                    circle(frame, Point(scatterX, scatterY), 1, Scalar(0, greenIntensity, 0), FILLED);
                }
            }
        } 

        // -- SAVE FRAME --
        string filename = "C:\\CPP\\radar\\case6\\frames\\frame_" + to_string(frameCount) + ".png";
        imwrite(filename, frame);

        // -- UPDATE PHYSICS --
        height = height - (dropVelocity * timeStep);
        frameCount++;
        
        if (frameCount % 30 == 0) {
            cout << "Saved 1 second of video... Height is now: " << height << "m" << endl;
        }
    }

    cout << "Finished! Total frames saved: " << frameCount << endl;

    return 0;
}