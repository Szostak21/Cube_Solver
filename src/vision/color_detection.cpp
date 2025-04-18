#include <color_detection.h>

ColorDetector::ColorDetector() {
}

ColorDetector::~ColorDetector() {

}

cv::Mat ColorDetector::preprocessImage(const cv::Mat &frame) {
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);   //convert BGR to HSV to use in different lightning 
    return hsv;
}

std::vector<cv::Rect> ColorDetector::detectColors(const cv::Mat &frame){

    cv::Mat hsv = preprocessImage(frame);

    std::vector<cv::Rect> detectedRegions;

    std::vector<std::pair<std::string, std::pair<cv::Scalar, cv::Scalar>>> colorRanges = {  //vector of pairs of colors and its ranges
        {"yellow", {lower_yellow, upper_yellow}},
        {"green", {lower_green, upper_green}},
        {"orange", {lower_orange, upper_orange}},
        {"blue", {lower_blue, upper_blue}},
        {"red", {lower_red, upper_red}},
        {"white", {lower_white, upper_white}}
    };

    for (const auto& color : colorRanges){
        cv::Mat mask;

        cv::inRange(hsv, color.second.first, color.second.second, mask); //check where the color is in the image

        cv::erode(mask, mask, cv::Mat(), cv::Point(-1, -1), 2); //removing noise and making color more visable
        cv::dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2); 

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); //find the contours of the color
        for (const auto& contour : contours){
            if (cv::contourArea(contour) > 500){
                cv::Rect boundingBox = cv::boundingRect(contour);
                detectedRegions.push_back(boundingBox);
            }
        }
    }
    return detectedRegions;
}