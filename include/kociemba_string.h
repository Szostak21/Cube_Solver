#ifndef KOCIEMBA_STRING_H
#define KOCIEMBA_STRING_H
#include <map>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

std::string buildKociembaString(const std::map<std::string, std::vector<std::string>>& clusteredStickers);

#endif