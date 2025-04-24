#ifndef COLOR_CLUSTERING_H
#define COLOR_CLUSTERING_H
#include <opencv2/opencv.hpp>
#include <map>
#include <string>
#include <vector>

class ColorClustering {
public:
    std::map<std::string, std::vector<cv::Vec<float, 3>>> assignColorsToCenters(const std::map<std::string, std::vector<cv::Vec<float, 3>>>& cubeSamples);
private:
    float hsvDistance(const cv::Vec<float, 3>& a, const cv::Vec<float, 3>& b);
    static bool compareByDistance(const std::pair<float, cv::Vec<float, 3>>& a, const std::pair<float, cv::Vec<float, 3>>& b);
};

#endif