#ifndef GRID_OVERLAY_H
#define GRID_OVERLAY_H

#include <opencv2/opencv.hpp>
#include <string>
#include "webcam_capture.h"

class GridOverlay {
public:
    GridOverlay(int startX, int startY, int size, const std::string& aboveColor, const std::string& centerColor, WebcamCapture& webcam);

    void drawGrid(cv::Mat& frame, const std::string& colorAbove, const std::string& colorCenter);

    void updateFaceLabels(const std::string& newAboveColor, const std::string& newCenterColor);

    void captureFace();

private:
    int gridStartX;
    int gridStartY;
    int gridSize;
    
    std::string aboveColor;
    std::string centerColor;

    WebcamCapture& webcamCapture;

    cv::Scalar colorYellow;
    cv::Scalar colorRed;
    cv::Scalar colorBlue;
    cv::Scalar colorWhite;
    cv::Scalar colorGreen;
    cv::Scalar colorOrange;

    void drawGridLines(cv::Mat& frame);

    void drawLabel(cv::Mat& frame, const std::string& label, const cv::Point& position, const cv::Scalar& color, int fontSize);
};

#endif
