#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <cmath>

using namespace cv;
using namespace std;

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
    double targetSizeMeters = 3.0; 
    double rps = 3.0; // 3 Revolutions Per Second
    
    double pi = 3.14159265;
    double fovRadians = fovDegrees * pi / 180.0;
    double tiltRadians = tiltDegrees * pi / 180.0;
    
    int frameCount = 0;
    
    // Initialize OpenCV Random Number Generator for the scattering effect
    RNG rng(12345);

    cout << "Starting Case 4 (Scattered Bright Spot) simulation..." << endl;

    // --- MAIN LOOP ---
    while (height > 0.0) {
        Mat frame(windowSize, windowSize, CV_8UC3, Scalar(20, 20, 20));

        // -- 1. WHAT THE RADAR SEES (FOV) --
        double groundWidthMeters = 2.0 * height * tan(fovRadians);
        
        // -- 2. DIGITAL SCREEN SCALING --
        double pixelsPerMeter = windowSize / groundWidthMeters;

        // -- 3. TARGET SIZING --
        int targetPixels = targetSizeMeters * pixelsPerMeter;
        
        // -- 4. UNDERSTANDING THE TILT & ROTATION --
        double shiftMeters = height * tan(tiltRadians);
        int shiftPixels = shiftMeters * pixelsPerMeter;

        double currentAngleRadians = (frameCount * timeStep) * (rps * 2.0 * pi);

        // -- 5. SCREEN COORDINATES --
        int centerX = windowSize / 2;
        int centerY = windowSize / 2;

        int shiftX = shiftPixels * cos(currentAngleRadians);
        int shiftY = shiftPixels * sin(currentAngleRadians);

        // Calculate the EXACT center point of the target (no need to find top-left anymore)
        int exactTargetCenterX = centerX + shiftX;
        int exactTargetCenterY = centerY + shiftY;

        // -- 6. DRAW THE SCATTERED BRIGHT SPOT (NOISE) --
        // Instead of a solid box, we draw hundreds of tiny points.
        // As the target gets bigger (closer), we increase the number of points so it stays bright.
        int numScatterPoints = targetPixels * 20; 

        for (int i = 0; i < numScatterPoints; i++) {
            // rng.gaussian pushes most points near the center, and scatters some further out.
            // We use (targetPixels / 3.0) as the "spread" size.
            int scatterX = exactTargetCenterX + rng.gaussian(targetPixels / 3.0);
            int scatterY = exactTargetCenterY + rng.gaussian(targetPixels / 3.0);

            // Only draw if the pixel is inside our window
            if (scatterX >= 0 && scatterX < windowSize && scatterY >= 0 && scatterY < windowSize) {
                // Draw a 1-pixel green dot. 
                // Using a slightly transparent/randomized green makes it look even more like noisy radar.
                int greenIntensity = 150 + (rng.uniform(0, 105)); // Random green between 150 and 255
                circle(frame, Point(scatterX, scatterY), 1, Scalar(0, greenIntensity, 0), FILLED);
            }
        }

        // -- SAVE FRAME --
        string filename = "C:\\CPP\\radar\\case4\\frames\\frame_" + to_string(frameCount) + ".png";
        imwrite(filename, frame);

        // -- 7. UPDATE PHYSICS FOR NEXT FRAME --
        height = height - (velocity * timeStep);
        frameCount++;
        
        if (frameCount % 30 == 0) {
            cout << "Saved 1 second of video... Height is now: " << height << "m" << endl;
        }
    }

    cout << "Finished! Total frames saved: " << frameCount << endl;

    return 0;
}