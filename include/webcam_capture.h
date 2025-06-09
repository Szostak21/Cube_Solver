#ifndef WEBCAM_CAPTURE_H
#define WEBCAM_CAPTURE_H

#include <opencv2/opencv.hpp>

class WebcamCapture {
public:
    WebcamCapture(int cameraIndex = 0); // 0 is the default camera

    ~WebcamCapture();

    bool captureFrame();

    cv::Mat getFrame();

    void showFrame(const std::string& windowName);

private:
    cv::VideoCapture cap;   
    cv::Mat frame; 
};

#endif
