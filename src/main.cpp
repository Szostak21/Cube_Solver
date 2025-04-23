#include <opencv2/opencv.hpp>
#include <iostream>
#include "webcam_capture.h"
#include "color_detection.h"
#include "grid_overlay.h"

int main() {
    WebcamCapture webcam(0);
    ColorDetector colorDetector;

    GridOverlay gridOverlay(100, 100, 150, "Red", "Yellow", webcam);

    std::string colorAbove = "Blue";
    std::string colorCenter = "Yellow";

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

        std::vector<std::pair<std::string, cv::Rect>> detectedColors = colorDetector.detectColors(frame);

        for (const auto& colorData : detectedColors) {
            const std::string& colorName = colorData.first;
            const cv::Rect& rect = colorData.second;

            cv::Scalar boxColor;
            if (colorName == "red")      boxColor = cv::Scalar(0, 0, 255);
            else if (colorName == "green")  boxColor = cv::Scalar(0, 255, 0);
            else if (colorName == "blue")   boxColor = cv::Scalar(255, 0, 0);
            else if (colorName == "yellow") boxColor = cv::Scalar(0, 255, 255);
            else if (colorName == "orange") boxColor = cv::Scalar(0, 165, 255);
            else if (colorName == "white")  boxColor = cv::Scalar(255, 255, 255);

            cv::rectangle(frame, rect, boxColor, 2);

            std::string label = colorName;
            // Optional: Show HSV values
            /*
            cv::Mat roi = frame(rect);
            cv::Mat hsvRoi;
            cv::cvtColor(roi, hsvRoi, cv::COLOR_BGR2HSV);
            cv::Scalar avgHsvColor = cv::mean(hsvRoi);
            label = colorName + " (" +
                    std::to_string(static_cast<int>(avgHsvColor[0])) + "," +
                    std::to_string(static_cast<int>(avgHsvColor[1])) + "," +
                    std::to_string(static_cast<int>(avgHsvColor[2])) + ")";
            */

            cv::putText(frame, label, cv::Point(rect.x, rect.y - 10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, boxColor, 2);
        }

        cv::imshow("Detected Colors", frame);

        int key = cv::waitKey(1);
        if (key == 27) break;

        if (key == ' ' || key == '\r') {
            if (colorCenter == "Yellow")      { colorCenter = "Green";  colorAbove = "Yellow"; }
            else if (colorCenter == "Green")  { colorCenter = "Orange"; colorAbove = "Yellow"; }
            else if (colorCenter == "Orange") { colorCenter = "Blue";   colorAbove = "Yellow"; }
            else if (colorCenter == "Blue")   { colorCenter = "Red";    colorAbove = "Yellow"; }
            else if (colorCenter == "Red")    { colorCenter = "White";  colorAbove = "Red";    }
            else if (colorCenter == "White")  { colorCenter = "Yellow"; colorAbove = "Blue";   }
        }
        
    }

    cv::destroyAllWindows();
    return 0;
}
