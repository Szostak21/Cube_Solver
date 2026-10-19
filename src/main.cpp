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
    } else {
        std::cout << std::endl << "Wybierz tryb" << std::endl << std::endl;
        std::cout << "[1] Znam notację kostki Rubika" << std::endl;
        std::cout << "[2] Nie wiem co to jest" << std::endl << std::endl;
    }
    std::cin >> choice;
    while (choice != 1 && choice != 2) {
        if (language == 1) {
            std::cout << "Please select a valid option [1] or [2]: " << std::endl << std::endl;
        } else {
            std::cout << "Proszę wybrać poprawną opcję [1] or [2]" << std::endl << std::endl;
        }
        std::cin >> choice;
    }

    WebcamCapture webcam(0);
    GridOverlay gridOverlay(100, 100, 150, "Red", "Yellow", webcam);
    ColorClustering colorClustering;

    std::map<std::string, std::vector<cv::Vec<float, 3>>> cubeSamples;
    std::string colorAbove = "Blue";
    std::string colorCenter = "Yellow";
    bool scanningDone = false;
    bool instructionsMode = false;
    std::vector<std::string> instructions;
    size_t currentStep = 0;

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

        if (instructionsMode && !instructions.empty()) {
            int frameWidth = frame.cols;
            int fontFace = cv::FONT_HERSHEY_DUPLEX;
            double fontScale = 0.4;
            int thickness = 1;
            std::string stepHdr;
            if (language == 1) {
                stepHdr = "Step " + std::to_string(currentStep + 1) + "/" + std::to_string(instructions.size()) + ": ";
            } else {
                stepHdr = "Krok " + std::to_string(currentStep + 1) + "/" + std::to_string(instructions.size()) + ": ";
            }
            std::string line = stepHdr + instructions[currentStep];
            cv::Size textSize = cv::getTextSize(line, fontFace, fontScale, thickness, nullptr);
            int y = std::max(textSize.height + 20, 30);
            cv::rectangle(frame, cv::Point((frameWidth - textSize.width) / 2 - 10, y - textSize.height - 10),
                          cv::Point((frameWidth + textSize.width) / 2 + 10, y + 10), cv::Scalar(0, 0, 0), cv::FILLED);
            cv::putText(frame, line, cv::Point((frameWidth - textSize.width) / 2, y), fontFace, fontScale, cv::Scalar(255, 255, 255), thickness);

            std::string hint = (language == 1) ? "SPACE/ENTER = next, ESC = exit" : "SPACJA/ENTER = dalej, ESC = wyjscie";
            double hintScale = 0.3;
            cv::Size hintSize = cv::getTextSize(hint, fontFace, hintScale, 1, nullptr);
            int hy = y + hintSize.height + 20;
            cv::rectangle(frame, cv::Point((frameWidth - hintSize.width) / 2 - 8, hy - hintSize.height - 8),
                          cv::Point((frameWidth + hintSize.width) / 2 + 8, hy + 8), cv::Scalar(0, 0, 0), cv::FILLED);
            cv::putText(frame, hint, cv::Point((frameWidth - hintSize.width) / 2, hy), fontFace, hintScale, cv::Scalar(255, 255, 255), 1);
        }

        cv::imshow("Rubik's Cube Grid Overlay", frame);

        int key = cv::waitKey(1);

        if (key == 27) {
            if (language == 1) {
                std::cout << "Exiting program. Goodbye!\n";
            } else {
                std::cout << "Zamykanie programu. Do widzenia!\n";
            }
            break;
        }

        if (instructionsMode && (key == ' ' || key == 13 || key == 'n' || key == 'N')) {
            if (currentStep + 1 < instructions.size()) {
                currentStep++;
            } else {
                instructionsMode = false;
            }
        }

        if (!scanningDone && (key == ' ' || key == '\r')) {
            std::vector<cv::Vec3f> samples = gridOverlay.captureFace();
            cubeSamples[colorCenter] = samples;
            if (colorCenter == "Yellow")      { colorCenter = "Green";  colorAbove = "Yellow"; }
            else if (colorCenter == "Green")  { colorCenter = "Orange"; colorAbove = "Yellow"; }
            else if (colorCenter == "Orange") { colorCenter = "Blue";   colorAbove = "Yellow"; }
            else if (colorCenter == "Blue")   { colorCenter = "Red";    colorAbove = "Yellow"; }
            else if (colorCenter == "Red")    { colorCenter = "White";  colorAbove = "Red"; }
            else if (colorCenter == "White")  {
                scanningDone = true;
                gridOverlay.setVisible(false);
            }
        }

        if (scanningDone && solution.empty()) {
            auto clusteredStickers = colorClustering.assignColorsToCenters(cubeSamples);
            std::string kociembaString = buildKociembaString(clusteredStickers);
            solution = run_python_kociemba(kociembaString);
            std::cout << std::endl;
            if (solution.empty() || solution.find("ValueError") != std::string::npos) {
                if (language == 1) {
                    std::cerr << "Error: Invalid cube arrangement. Please scan the cube again. (Try in better light)" << std::endl;
                } else {
                    std::cerr << "Błąd: Nieprawidłowe ułożenie kostki. Proszę zeskanować kostkę ponownie. (Spróbuj w lepszym świetle)\n" << kociembaString << std::endl;
                }
                solution.clear();
                cubeSamples.clear();
                scanningDone = false;
                gridOverlay.setVisible(true);
                colorAbove = "Blue";
                colorCenter = "Yellow";
            } else {
                if (language == 1) {
                    std::cout << "- - - - - - - - -" << std::endl;
                    std::cout << "|  S O L V E D  |" << std::endl;
                    std::cout << "- - - - - - - - -" << std::endl << std::endl;
                } else {
                    std::cout << "- - - - - - - - - - - - -" << std::endl;
                    std::cout << "|  R O Z W I Ą Z A N O  |" << std::endl;
                    std::cout << "- - - - - - - - - - - - -" << std::endl << std::endl;
                }
                if (choice == 1) {
                    if (language == 1) {
                        std::cout << "ORANGE center in front, YELLOW center on top" << std::endl;
                        std::cout << "Solution: " << solution << std::endl;
                    } else {
                        std::cout << "POMARAŃCZOWY środek z przodu, ŻÓŁTY środek na górze" << std::endl;
                        std::cout << "Rozwiązanie: " << solution << std::endl;
                    }
                } else if (choice == 2) {
                    instructions.clear();
                    currentStep = 0;
                    instructionsMode = true;
                    for (size_t i = 0; i < solution.size(); ++i) {
                        char c = solution[i];
                        if (c != 'U' && c != 'D' && c != 'L' && c != 'R' && c != 'F' && c != 'B') continue;
                        char modifier = '1';
                        if (i + 1 < solution.size() && (solution[i + 1] == '\'' || solution[i + 1] == '2')) {
                            modifier = solution[i + 1];
                            ++i;
                        }
                        std::string instr;
                        if (language == 1) {
                            if (c == 'U') instr = "YELLOW center clockwise";
                            else if (c == 'D') instr = "WHITE center clockwise";
                            else if (c == 'L') instr = "GREEN center clockwise";
                            else if (c == 'R') instr = "BLUE center clockwise";
                            else if (c == 'F') instr = "ORANGE center clockwise";
                            else if (c == 'B') instr = "RED center clockwise";
                            if (modifier == '\'') instr += " THREE times";
                            else if (modifier == '2') instr += " TWICE";
                        } else {
                            if (c == 'U') instr = "ZOLTY srodek zgodnie z ruchem wskazowek zegara";
                            else if (c == 'D') instr = "BIALY srodek zgodnie z ruchem wskazowek zegara";
                            else if (c == 'L') instr = "ZIELONY srodek zgodnie z ruchem wskazowek zegara";
                            else if (c == 'R') instr = "NIEBIESKI srodek zgodnie z ruchem wskazowek zegara";
                            else if (c == 'F') instr = "POMARANCZOWY srodek zgodnie z ruchem wskazowek zegara";
                            else if (c == 'B') instr = "CZERWONY srodek zgodnie z ruchem wskazowek zegara";
                            if (modifier == '\'') instr += " TRZY razy";
                            else if (modifier == '2') instr += " DWA razy";
                        }
                        instructions.push_back(instr);
                    }
                }
            }
        }
    }

    cv::destroyAllWindows();
    return 0;
}