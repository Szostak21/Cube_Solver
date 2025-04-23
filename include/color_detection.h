#ifndef COLOR_DETECTION_H
#define COLOR_DETECTION_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string> 

class ColorDetector {
public:
    ColorDetector();
    ~ColorDetector();

    std::vector<std::pair<std::string, cv::Rect>> detectColors(const cv::Mat &frame);

    cv::Mat preprocessImage(const cv::Mat &frame);

private:
    // RED (wraps around hue 0)
cv::Scalar lower_red1 = cv::Scalar(170, 210, 60);
cv::Scalar upper_red1 = cv::Scalar(180, 255, 90);
cv::Scalar lower_red2 = cv::Scalar(0, 210, 60);
cv::Scalar upper_red2 = cv::Scalar(5, 255, 90);

// ORANGE
cv::Scalar lower_orange = cv::Scalar(5, 180, 140);
cv::Scalar upper_orange = cv::Scalar(25, 240, 170);

// YELLOW
cv::Scalar lower_yellow = cv::Scalar(35, 180, 120);
cv::Scalar upper_yellow = cv::Scalar(50, 210, 180);

// GREEN
cv::Scalar lower_green = cv::Scalar(65, 230, 90);
cv::Scalar upper_green = cv::Scalar(75, 255, 125);

// BLUE
cv::Scalar lower_blue = cv::Scalar(100, 220, 90);
cv::Scalar upper_blue = cv::Scalar(105, 255, 120);

// WHITE (low S, high V)
cv::Scalar lower_white = cv::Scalar(85, 20, 100);
cv::Scalar upper_white = cv::Scalar(105, 60, 145);



    std::vector<std::pair<std::string, std::pair<cv::Scalar, cv::Scalar>>> colorRanges;
};

#endif