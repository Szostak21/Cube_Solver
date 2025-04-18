#include "webcam_capture.h"
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    WebcamCapture webcam;

    while (true) {
        if (webcam.captureFrame()) {
            webcam.showFrame("Webcam Feed");
        } else {
            std::cerr << "Error: Failed to capture frame!" << std::endl;
            break;
        }

        if (cv::waitKey(1) == 27) {
            break;  
        }
    }

    cv::destroyAllWindows();
    return 0;
}
