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

    size_t language, choice;
    std::string solution;
    std::cout << std::endl << "Welcome to the Rubik's Cube Solver!" << std::endl << std::endl;
    std::cout << std::endl << "Select language";
    std::cout << std::endl << "Wybierz język: " << std::endl << std::endl;
    std::cout << "[1] English" << std::endl;
    std::cout << "[2] Polski" << std::endl << std::endl;
    std::cin >> language;
    while (language != 1 && language != 2) {
        std::cout << "Please select a valid option [1] or [2]" << std::endl;
        std::cout << "Proszę wybrać poprawną opcję [1] or [2]: " << std::endl << std::endl; 
        std::cin >> language;
    }
    if (language == 1) {
        std::cout << std::endl << "Select mode" << std::endl << std::endl;
        std::cout << "[1] I know the Rubik's Cube notation" << std::endl;
        std::cout << "[2] I don't know what that is" << std::endl << std::endl;
    }
    else{
        std::cout << std::endl << "Wybierz tryb" << std::endl << std::endl;
        std::cout << "[1] Znam notację kostki Rubika" << std::endl;
        std::cout << "[2] Nie wiem co to jest" << std::endl << std::endl;
    }
    std::cin >> choice;
    while (choice != 1 && choice != 2) {
        if (language == 1) {
            std::cout << "Please select a valid option [1] or [2]: " << std::endl << std::endl;
        } 
        else{
            std::cout << "Proszę wybrać poprawną opcję [1] or [2]" << std::endl << std::endl;
        }
        std::cin >> choice;
    }
    while (true) {
        WebcamCapture webcam(0);
        GridOverlay gridOverlay(100, 100, 150, "Red", "Yellow", webcam);
        ColorClustering colorClustering;

        std::map<std::string, std::vector<cv::Vec<float, 3>>> cubeSamples;
        std::string colorAbove = "Blue";
        std::string colorCenter = "Yellow";

        while (true) {
            if (!webcam.captureFrame()) {
                if (language == 1) {
                    std::cerr << "Failed to capture frame. Exiting..." << std::endl;
                } else {
                    std::cerr << "Nie udało się przechwycić klatki. Wychodzenie..." << std::endl;
                }
                break;
            }

            cv::Mat frame = webcam.getFrame();
            cv::flip(frame, frame, 1);

            if (frame.empty()) {
                if (language == 1) {
                    std::cerr << "Empty frame received. Skipping..." << std::endl;
                } else {
                    std::cerr << "Otrzymano pustą klatkę. Pomijanie..." << std::endl;
                }
                continue;
            }

            gridOverlay.drawGrid(frame, colorAbove, colorCenter, language);

            int key = cv::waitKey(1);

            if (key == 27) {cv::destroyAllWindows(); return 0;} // ESC key to exit;
            if (key == ' ' || key == '\r') {
                std::vector<cv::Vec3f> samples = gridOverlay.captureFace();
                cubeSamples[colorCenter] = samples;
                if (colorCenter == "Yellow")      { colorCenter = "Green";  colorAbove = "Yellow"; }
                else if (colorCenter == "Green")  { colorCenter = "Orange"; colorAbove = "Yellow"; }
                else if (colorCenter == "Orange") { colorCenter = "Blue";   colorAbove = "Yellow"; }
                else if (colorCenter == "Blue")   { colorCenter = "Red";    colorAbove = "Yellow"; }
                else if (colorCenter == "Red")    { colorCenter = "White";  colorAbove = "Red"; }
                else if (colorCenter == "White")  { colorCenter = ""; colorAbove = "";   break;}
            }
            cv::imshow("Rubik's Cube Grid Overlay", frame);
        }

        std::map<std::string, std::vector<std::string>> clusteredStickers = colorClustering.assignColorsToCenters(cubeSamples);
        std::string kociembaString = buildKociembaString(clusteredStickers);
        solution = run_python_kociemba(kociembaString);
        cv::destroyAllWindows();
        std::cout << std::endl;
        if (solution.empty() || solution.find("ValueError") != std::string::npos) {
            if (language == 1) {
                std::cerr << "Error: Invalid cube arrangement. Please scan the cube again. (Try in better light)" << std::endl;
            } else {
                std::cerr << "Błąd: Nieprawidłowe ułożenie kostki. Proszę zeskanować kostkę ponownie. (Spróbuj w lepszym świetle)" << std::endl;
            }
            continue;
        }
        else {
            if (language == 1) {
                std::cout << "- - - - - - - - -" << std::endl;
                std::cout << "|  S O L V E D  |" << std::endl;
                std::cout << "- - - - - - - - -" << std::endl << std::endl;
            } else {
                std::cout << "- - - - - - - - - - - - -" << std::endl;
                std::cout << "|  R O Z W I Ą Z A N O  |" << std::endl;
                std::cout << "- - - - - - - - - - - - -" << std::endl << std::endl;
            }
            break;
        }
    }
    if (choice == 1){    
        if (language == 1) {
            std::cout << "ORANGE center in front, YELLOW center on top" << std::endl;
            std::cout << "Solution: " << solution << std::endl;
        } 
        else {
            std::cout << "POMARAŃCZOWY środek z przodu, ŻÓŁTY środek na górze" << std::endl;
            std::cout << "Rozwiązanie: " << solution << std::endl;
        }
    }
    
    if (choice == 2){
        if (language == 1) {
            std::cin.ignore();
            std::cout << "Turn the sides with the CENTER piece of the color as shown: " << std::endl << std::endl;
            std::cout << "Press ENTER twice to show next move" << std::endl << std::endl;
            for (char c : solution) {
                if (c != '\'' && c != '2'){
                    std::cin.get();
                    if (c == 'U') std::cout << std::endl << " YELLOW clockwise ";
                    else if (c == 'D') std::cout << std::endl << " WHITE clockwise ";
                    else if (c == 'L') std::cout << std::endl << " GREEN clockwise ";
                    else if (c == 'R') std::cout << std::endl << " BLUE clockwise ";
                    else if (c == 'F') std::cout << std::endl << " ORANGE clockwise ";
                    else if (c == 'B') std::cout << std::endl << " RED clockwise ";
                }
                else if (c == '\'') {std::cout << "THREE times";}
                else if (c == '2') {std::cout << "TWICE";} 
                else{
                    continue;
                }
            }
            std::cout << std::endl << std::endl << "Thanks for using Cube Solver!" << std::endl;
        } 
        else {
            std::cin.ignore();
            std::cout << "Obróć sciany ze wskazanym kolorem ŚRODKA w ten sposób: " << std::endl << std::endl;
            std::cout << "Naciśnij ENTER dwukrotnie aby wyświetlić następny ruch" << std::endl << std::endl;
            for (char c : solution) {
                if (c != '\'' && c != '2'){
                    std::cin.get();
                    if (c == 'U') std::cout << std::endl << " ŻÓŁTY zgodnie z ruchem wskazówek zegara ";
                    else if (c == 'D') std::cout << std::endl << " BIAŁY zgodnie z ruchem wskazówek zegara ";
                    else if (c == 'L') std::cout << std::endl << " ZIELONY zgodnie z ruchem wskazówek zegara ";
                    else if (c == 'R') std::cout << std::endl << " NIEBIESKI zgodnie z ruchem wskazówek zegara ";
                    else if (c == 'F') std::cout << std::endl << " POMARAŃCZOWY zgodnie z ruchem wskazówek zegara ";
                    else if (c == 'B') std::cout << std::endl << " CZERWONY zgodnie z ruchem wskazówek zegara ";
                }
                else if (c == '\'') {std::cout << "TRZY razy";}
                else if (c == '2') {std::cout << "DWA razy";} 
                else{
                    continue;
                }    
            }
            std::cout << std::endl << std::endl << "Dzieki za skorzystanie z programu!" << std::endl;
        }
    }    
    std::cout << std::endl;    
    return 0;
}