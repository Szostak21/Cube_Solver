#include <opencv2/opencv.hpp>
#include <iostream>

cv::Mat currentHSV;

void onMouse(int event, int x, int y, int, void*) {
    if (event == cv::EVENT_LBUTTONDOWN && !currentHSV.empty()) {
        cv::Vec3b hsv = currentHSV.at<cv::Vec3b>(y, x);
        std::cout << "Clicked HSV at (" << x << ", " << y << "): ["
                  << (int)hsv[0] << ", " << (int)hsv[1] << ", " << (int)hsv[2] << "]" << std::endl;
    }
}

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open camera." << std::endl;
        return -1;
    }

    cv::namedWindow("HSV Sampler");
    cv::setMouseCallback("HSV Sampler", onMouse);

    while (true) {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty()) continue;

        cv::cvtColor(frame, currentHSV, cv::COLOR_BGR2HSV);
        cv::imshow("HSV Sampler", frame);

        if (cv::waitKey(1) == 27) break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
