#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include "webcam_capture.h"
#include "grid_overlay.h"
#include "color_clustering.h"
#include "kociemba_string.h"
#include "kociemba.h"

int main() {

    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    size_t choice;
    std::cout << std::endl << "Welcome to the Rubik's Cube Solver!" << std::endl << std::endl;
    std::cout << "[1] I KNOW the Rubik's Cube notation" << std::endl;
    std::cout << "[2] I DON'T know what that is" << std::endl << std::endl;
    std::cin >> choice;
        while (choice != 1 && choice != 2) {
        std::cout << "Please select a valid option [1] or [2]: ";
        std::cin >> choice;
    }
    while (true){
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

        if (key == 27) {cv::destroyAllWindows(); return 0;} // ESC key to exit;
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
        cv::imshow("Rubik's Cube Grid Overlay", frame); //this produces the QSocketNotifier error
    }

    std::map<std::string, std::vector<std::string>> clusteredStickers = colorClustering.assignColorsToCenters(cubeSamples);
    std::string kociembaString = buildKociembaString(clusteredStickers);
    std::string solution = run_python_kociemba(kociembaString);
    cv::destroyAllWindows();
    std::cout << std::endl << std::endl;
    if (solution.empty() || solution.find("ValueError") != std::string::npos) {
        std::cerr << "Error: Invalid cube arrangement. Please scan the cube again. (Try in better light)" << std::endl;
    }
    else{
        std::cout << "- - - - - - - - -" << std::endl;
        std::cout << "|  S O L V E D  |" << std::endl;
        std::cout << "- - - - - - - - -" << std::endl << std::endl << std::endl;

    if (choice == 1){    
    std::cout << "ORANGE center in front, YELLOW center on top" << std::endl;
    std::cout << "Solution: " << solution << std::endl;
    }
    
    if (choice == 2){
        std::cout << "Turn the sides with the CENTER piece of the color as shown: " << std::endl;
        std:;size_t moveCount = 0;
        for (char c : solution) {
            moveCount++;
            if (c == 'U') std::cout << std::endl << moveCount << " Yellow clockwise ";
            else if (c == 'D') std::cout << std::endl << moveCount << " White clockwise ";
            else if (c == 'L') std::cout << std::endl << moveCount << " Green clockwise ";
            else if (c == 'R') std::cout << std::endl << moveCount << " Blue clockwise ";
            else if (c == 'F') std::cout << std::endl << moveCount << " Orange clockwise ";
            else if (c == 'B') std::cout << std::endl << moveCount << " Red clockwise ";

            else if (c == '\'') {std::cout << "three times"; moveCount--;}
            else if (c == '2') {std::cout << "twice"; moveCount--;} 
            else moveCount--;
        }
    }    
    std::cout << std::endl;
    break;
    }
    }
    return 0;
}
