#include <opencv2/opencv.hpp>
#include <iostream>
#include "webcam_capture.h"
#include "grid_overlay.h"
#include "color_clustering.h"
int main() {
    WebcamCapture webcam(0);
    GridOverlay gridOverlay(100, 100, 150, "Red", "Yellow", webcam);
    ColorClustering colorClustering;

    std::map<std::string, std::vector<cv::Vec<float, 3>>> cubeSamples;
    std::string colorAbove = "Blue";
    std::string colorCenter = "Yellow";
    
    std::map<std::string, std::string> faceOrder = {
        {"Yellow", "Blue"},  // U, above is Blue
        {"Green", "Yellow"}, // L, above is Yellow
        {"Orange", "Yellow"}, // F, above is Yellow
        {"Blue", "Yellow"}, // R, above is Yellow
        {"Red", "Yellow"},  // B, above is Yellow
        {"White", "Red"}    // D, above is Red
    };

    while (true) {
        if (!webcam.captureFrame()) {
            std::cerr << "Failed to capture frame. Exiting..." << std::endl;
            break;
        }

        cv::Mat frame = webcam.getFrame();

        if (frame.empty()) {
            std::cerr << "Empty frame received. Skipping..." << std::endl;
            continue;
        }

        gridOverlay.drawGrid(frame, colorAbove, colorCenter);

        int key = cv::waitKey(1);

        if (key == 27) break;

        if (key == ' ' || key == '\r') {
            std::vector<cv::Vec3f> samples = gridOverlay.captureFace();
            cubeSamples[colorCenter] = samples;

            if (colorCenter == "Yellow")      { colorCenter = "Green";  colorAbove = "Yellow"; }
            else if (colorCenter == "Green")  { colorCenter = "Orange"; colorAbove = "Yellow"; }
            else if (colorCenter == "Orange") { colorCenter = "Blue";   colorAbove = "Yellow"; }
            else if (colorCenter == "Blue")   { colorCenter = "Red";    colorAbove = "Yellow"; }
            else if (colorCenter == "Red")    { colorCenter = "White";  colorAbove = "Orange"; }
            else if (colorCenter == "White")  { colorCenter = ""; colorAbove = "";   break;}
        }

        cv::imshow("Rubik's Cube Grid Overlay", frame);
    }

    std::map<std::string, std::vector<cv::Vec<float, 3>>> clusteredSamples = colorClustering.assignColorsToCenters(cubeSamples);

    std::cout << "Clustered Samples:" << std::endl;
    for (const auto& entry : clusteredSamples) {
        std::cout << "Face: " << entry.first << std::endl;
        for (const auto& sample : entry.second) {
            std::cout << "  HSV: (" << static_cast<int>(sample[0]) << ", "
                                    << static_cast<int>(sample[1]) << ", "
                                    << static_cast<int>(sample[2]) << ")" << std::endl;
        }
    }

    cv::destroyAllWindows();    
    return 0;
}
