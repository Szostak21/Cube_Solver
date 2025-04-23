#include "grid_overlay.h"

GridOverlay::GridOverlay(int startX, int startY, int size, const std::string& aboveColor, const std::string& centerColor, WebcamCapture& webcam)
    : gridStartX(startX), gridStartY(startY), gridSize(size),
      aboveColor(aboveColor), centerColor(centerColor), webcamCapture(webcam),
      colorYellow(0, 255, 255), colorRed(0, 0, 255), colorBlue(255, 0, 0),
      colorWhite(255, 255, 255), colorGreen(0, 255, 0), colorOrange(0, 165, 255) {}

void GridOverlay::drawGrid(cv::Mat& frame, const std::string& colorAbove, const std::string& colorCenter) {
    int frameWidth = frame.cols;
    int frameHeight = frame.rows;

    gridSize = std::min(frameWidth, frameHeight) * 0.4;
    gridStartX = (frameWidth - gridSize) / 2;
    gridStartY = (frameHeight - gridSize) / 2;

    int cellSize = gridSize / 3;

    for (int i = 0; i <= 3; ++i) {
        int offset = i * cellSize;
        cv::line(frame, cv::Point(gridStartX + offset, gridStartY),
                        cv::Point(gridStartX + offset, gridStartY + gridSize), colorWhite, 2);
        cv::line(frame, cv::Point(gridStartX, gridStartY + offset),
                        cv::Point(gridStartX + gridSize, gridStartY + offset), colorWhite, 2);
    }

    int fontFace = cv::FONT_HERSHEY_TRIPLEX;
    double fontScale = 0.7;
    int thickness = 2;
    int baseline = 0;

    std::string aboveLabel = "Above: " + colorAbove;
    std::string centerLabel = "Center: " + colorCenter;

    cv::Size aboveSize = cv::getTextSize(aboveLabel, fontFace, fontScale, thickness, &baseline);
    cv::Size centerSize = cv::getTextSize(centerLabel, fontFace, fontScale, thickness, &baseline);

    int aboveY = std::max(aboveSize.height + 10, gridStartY - 10);
    int centerY = std::min(frameHeight - 10, gridStartY + gridSize + centerSize.height + 10);

    cv::putText(frame, aboveLabel,
                cv::Point((frameWidth - aboveSize.width) / 2, aboveY),
                fontFace, fontScale, colorWhite, thickness);

    cv::putText(frame, centerLabel,
                cv::Point((frameWidth - centerSize.width) / 2, centerY),
                fontFace, fontScale, colorWhite, thickness);
}

void GridOverlay::updateFaceLabels(const std::string& newAboveColor, const std::string& newCenterColor) {
    aboveColor = newAboveColor;
    centerColor = newCenterColor;
}

void GridOverlay::captureFace() {
    // Placeholder for capturing logic, if needed later
}
