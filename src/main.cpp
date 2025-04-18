#include <opencv2/opencv.hpp>
#include <iostream>
#include "webcam_capture.h"
#include "color_detection.h"

int main() {
    WebcamCapture webcam(0);
    ColorDetector colorDetector;

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

        std::vector<std::pair<std::string, cv::Rect>> detectedColors = colorDetector.detectColors(frame);

        for (const auto& colorData : detectedColors) {
            const std::string& colorName = colorData.first;
            const cv::Rect& rect = colorData.second;

            cv::Scalar boxColor;

            if (colorName == "red") {
                boxColor = cv::Scalar(0, 0, 255); 
            } else if (colorName == "green") {
                boxColor = cv::Scalar(0, 255, 0);
            } else if (colorName == "blue") {
                boxColor = cv::Scalar(255, 0, 0);
            } else if (colorName == "yellow") {
                boxColor = cv::Scalar(0, 255, 255);
            } else if (colorName == "orange") {
                boxColor = cv::Scalar(0, 165, 255);
            } else if (colorName == "white") {
                boxColor = cv::Scalar(255, 255, 255);
            }

            cv::rectangle(frame, rect, boxColor, 2);
            std::string label = colorName;
            
/*          //uncomment this if you want to show the average color
            cv::Mat roi = frame(rect);
            cv::Scalar avgColor = cv::mean(roi);

            label = colorName + " (" +
                                std::to_string(static_cast<int>(avgColor[2])) + "," +  // R
                                std::to_string(static_cast<int>(avgColor[1])) + "," +  // G
                                std::to_string(static_cast<int>(avgColor[0])) + ")";  // B
*/

            cv::putText(frame, label, cv::Point(rect.x, rect.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.7, boxColor, 2);
        }
        cv::imshow("Detected Colors", frame);

        if (cv::waitKey(1) == 27) {
            break;
        }
    }

    cv::destroyAllWindows();
    return 0;
}