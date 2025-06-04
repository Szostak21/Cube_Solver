#include <opencv2/opencv.hpp>
#include <iostream>
#include "webcam_capture.h"
#include "grid_overlay.h"
#include "color_clustering.h"
#include "kociemba_string.h"

std::string run_python_kociemba(const std::string& kociembaString) {
    std::string command = "python3 -c \"import kociemba; print(kociemba.solve('" + kociembaString + "'))\" 2>/dev/null";
    std::array<char, 128> buffer;
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);
    if (!result.empty() && result[result.size()-1] == '\n') result.pop_back();
    return result;
}

int main() {
    WebcamCapture webcam(0);
    GridOverlay gridOverlay(100, 100, 150, "Red", "Yellow", webcam);
    ColorClustering colorClustering;

    std::map<std::string, std::vector<cv::Vec<float, 3>>> cubeSamples;
    std::string colorAbove = "Blue";
    std::string colorCenter = "Yellow";

    while (true) {
        if (!webcam.captureFrame()) {
            std::cerr << "Failed to capture frame. Exiting..." << std::endl;
            break;
        }

        cv::Mat frame = webcam.getFrame();

        if (frame.empty()) {
            std::cerr << "Empty frame received. Skipping..." << std::endl;
            continue;
        }

        gridOverlay.drawGrid(frame, colorAbove, colorCenter);

        int key = cv::waitKey(1);

        if (key == 27) break;

        if (key == ' ' || key == '\r') {
            std::vector<cv::Vec3f> samples = gridOverlay.captureFace();
            cubeSamples[colorCenter] = samples;

            if (colorCenter == "Yellow")      { colorCenter = "Green";  colorAbove = "Yellow"; }
            else if (colorCenter == "Green")  { colorCenter = "Orange"; colorAbove = "Yellow"; }
            else if (colorCenter == "Orange") { colorCenter = "Blue";   colorAbove = "Yellow"; }
            else if (colorCenter == "Blue")   { colorCenter = "Red";    colorAbove = "Yellow"; }
            else if (colorCenter == "Red")    { colorCenter = "White";  colorAbove = "Orange"; }
            else if (colorCenter == "White")  { colorCenter = ""; colorAbove = "";   break;}
        }

        cv::imshow("Rubik's Cube Grid Overlay", frame);
    }

    std::map<std::string, std::vector<std::string>> clusteredStickers = colorClustering.assignColorsToCenters(cubeSamples);

    std::cout << "Clustered Stickers:" << std::endl;
    for (const auto& entry : clusteredStickers) {
        std::cout << "Face: " << entry.first << std::endl;
        for (const auto& sticker : entry.second) {
            std::cout << "  " << sticker << std::endl;
        }
    }

    std::string kociembaString = buildKociembaString(clusteredStickers);
    std::cout << "Kociemba String: " << kociembaString << std::endl;
    std::string solution = run_python_kociemba(kociembaString);
    if (solution.empty() || solution.find("ValueError") != std::string::npos) {
        std::cerr << "Error: Invalid Kociemba string. Please scan the cube again." << std::endl;
        return 1;
}
    std::cout << "Kociemba Solution: " << solution << std::endl;

    cv::destroyAllWindows();
    return 0;
}
