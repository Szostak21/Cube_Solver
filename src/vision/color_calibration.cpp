#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

cv::Mat currentHSV;
std::vector<cv::Vec<float, 3>> hsvSamples;

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

        // Sample the HSV values
        std::vector<float> h, s, v;
        for (int y = 0; y < currentHSV.rows; ++y)
            for (int x = 0; x < currentHSV.cols; ++x) {
                cv::Vec3b pix = currentHSV.at<cv::Vec3b>(y, x);
                h.push_back(pix[0]);
                s.push_back(pix[1]);
                v.push_back(pix[2]);
            }
        std::nth_element(h.begin(), h.begin() + h.size()/2, h.end());
        std::nth_element(s.begin(), s.begin() + s.size()/2, s.end());
        std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
        hsvSamples.push_back(cv::Vec<float, 3>(h[h.size()/2], s[s.size()/2], v[v.size()/2]));

        cv::imshow("HSV Sampler", frame);

        if (cv::waitKey(1) == 27) break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
