#ifndef COLOR_DETECTION_H
#define COLOR_DETECTION_H

#include <opencv2/opencv.hpp>
#include <vector>

class ColorDetector {
public:
    ColorDetector();
    ~ColorDetector();

    std::vector<cv::Rect> detectColors(const cv::Mat &frame);

    cv::Mat preprocessImage(const cv::Mat &frame);

private:
    cv::Scalar lower_red = cv::Scalar(0, 120, 70);
    cv::Scalar upper_red = cv::Scalar(10, 255, 255);
    cv::Scalar lower_green = cv::Scalar(40, 70, 70);
    cv::Scalar upper_green = cv::Scalar(90, 255, 255);
    cv::Scalar lower_blue = cv::Scalar(100, 150, 0);
    cv::Scalar upper_blue = cv::Scalar(140, 255, 255);
    cv::Scalar lower_yellow = cv::Scalar(20, 100, 100);
    cv::Scalar upper_yellow = cv::Scalar(30, 255, 255);
    cv::Scalar lower_orange = cv::Scalar(10, 100, 20);
    cv::Scalar upper_orange = cv::Scalar(25, 255, 255);
    cv::Scalar lower_white = cv::Scalar(0, 0, 200);
    cv::Scalar upper_white = cv::Scalar(180, 20, 255);

};



#endif