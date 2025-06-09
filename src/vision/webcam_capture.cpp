#include <webcam_capture.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>

WebcamCapture::WebcamCapture(int cameraIndex) {
    cap.open(cameraIndex);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera." << std::endl;  //check if camera opened
    }
}

WebcamCapture::~WebcamCapture() {
    cap.release();
}

bool WebcamCapture::captureFrame() {
    if (cap.read(frame)){
        return true;
    } 
    else {
        std::cerr << "Error: Could not capture frame." << std::endl;
        return false;
    }
}

cv::Mat WebcamCapture::getFrame() {
    return frame;
}

void WebcamCapture::showFrame(const std::string& windowName) {
    cv::imshow(windowName, frame);
}