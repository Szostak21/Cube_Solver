#include "color_detection.h"
#include <opencv2/opencv.hpp>

ColorDetector::ColorDetector() {
    colorRanges = {
        {"yellow", {lower_yellow, upper_yellow}},
        {"green", {lower_green, upper_green}},
        {"orange", {lower_orange, upper_orange}},
        {"blue", {lower_blue, upper_blue}},
        {"red", {lower_red, upper_red}},
        {"white", {lower_white, upper_white}}
    };
}

ColorDetector::~ColorDetector() {
    
}

cv::Mat ColorDetector::preprocessImage(const cv::Mat &frame) {
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);  // Convert BGR to HSV
    return hsv;
}

std::vector<std::pair<std::string, cv::Rect>> ColorDetector::detectColors(const cv::Mat &frame) {
    cv::Mat hsv = preprocessImage(frame);
    std::vector<std::pair<std::string, cv::Rect>> detectedRegions;

    for (const auto& color : colorRanges) {
        cv::Mat mask;
        cv::inRange(hsv, color.second.first, color.second.second, mask);

        cv::erode(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);
        cv::dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (const auto& contour : contours) {
            if (cv::contourArea(contour) > 500) {
                cv::Rect boundingBox = cv::boundingRect(contour);
                detectedRegions.push_back({color.first, boundingBox});
            }
        }
    }

    return detectedRegions;
}
