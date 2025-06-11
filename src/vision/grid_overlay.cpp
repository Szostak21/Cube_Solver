#include "grid_overlay.h"
#include <map>

GridOverlay::GridOverlay(int startX, int startY, int size, const std::string& aboveColor, const std::string& centerColor, WebcamCapture& webcam)
    : gridStartX(startX), gridStartY(startY), gridSize(size),
      aboveColor(aboveColor), centerColor(centerColor), webcamCapture(webcam),
      colorYellow(0, 255, 255), colorRed(0, 0, 255), colorBlue(255, 0, 0),
      colorWhite(255, 255, 255), colorGreen(0, 255, 0), colorOrange(0, 165, 255) {}

void GridOverlay::drawGrid(cv::Mat& frame, const std::string& colorAbove, const std::string& colorCenter, int language) {
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

    int fontFace = cv::FONT_HERSHEY_DUPLEX;
    double fontScale = 0.7;
    float thickness = 1.5;
    int baseline = 0;

    std::string aboveLabel, centerLabel;
    std::string aboveColorLabel = translateColorName(colorAbove, language);
    std::string centerColorLabel = translateColorName(colorCenter, language);

    if (language == 1) {
        aboveLabel = "CENTER piece of the TOP face: " + aboveColorLabel;
        centerLabel = "CENTER piece of the SCANNED face: " + centerColorLabel;
    } else {
        aboveLabel = "Kolor SRODKA GORNEJ sciany: " + aboveColorLabel;
        centerLabel = "Kolor SRODKA SKANOWANEJ sciany: " + centerColorLabel;
    }

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

    std::string scanLabel;
    if (language == 1) {
        scanLabel = "Press SPACE to scan";
    } else {
        scanLabel = "Nacisnij SPACJE aby zeskanowac";
    }
    cv::Size scanSize = cv::getTextSize(scanLabel, fontFace, fontScale, thickness, &baseline);
    int scanY = centerY + scanSize.height + 15;

    cv::putText(frame, scanLabel,
                cv::Point((frameWidth - scanSize.width) / 2, scanY),
                fontFace, fontScale, colorWhite, thickness);
}

void GridOverlay::updateFaceLabels(const std::string& newAboveColor, const std::string& newCenterColor) {
    aboveColor = newAboveColor;
    centerColor = newCenterColor;
}

std::vector<cv::Vec<float, 3>> GridOverlay::captureFace() {
    cv::Mat frame = webcamCapture.getFrame();

    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    int cellSize = gridSize / 3;
    std::vector<cv::Vec<float, 3>> hsvSamples;

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            int mirroredCol = 2 - col;
            int cx = gridStartX + mirroredCol * cellSize + cellSize / 2;
            int cy = gridStartY + row * cellSize + cellSize / 2;

            cv::Rect region(cx - 2, cy - 2, 5, 5);
            cv::Mat patch = hsv(region);

            cv::Scalar avg = cv::mean(patch);
            hsvSamples.push_back(cv::Vec<float, 3>(avg[0], avg[1], avg[2]));
        }
    }
    return hsvSamples;
}

std::string GridOverlay::translateColorName(const std::string& color, int language) const {
    static const std::map<std::string, std::string> enToPl = {
        {"Yellow", "Zolty"},
        {"Green", "Zielony"},
        {"Orange", "Pomaranczowy"},
        {"Blue", "Niebieski"},
        {"Red", "Czerwony"},
        {"White", "Bialy"}
    };
    if (language == 1) {
        return color;
    } else {
        auto it = enToPl.find(color);
        if (it != enToPl.end()) return it->second;
        return color;
    }
}
