#include "color_clustering.h"
#include <limits>
#include <algorithm>

float ColorClustering::hsvDistance(const cv::Vec<float, 3>& a, const cv::Vec<float, 3>& b) {
    float dh = std::min(std::abs(a[0] - b[0]), 180.0f - std::abs(a[0] - b[0]));
    float ds = std::abs(a[1] - b[1]);
    float dv = std::abs(a[2] - b[2]);
    return std::sqrt(dh * dh + ds * ds + dv * dv);
}

bool ColorClustering::compareByDistance(const std::pair<float, cv::Vec<float, 3>>& a,
    const std::pair<float, cv::Vec<float, 3>>& b) {
    return a.first < b.first;
}

std::map<std::string, std::vector<std::string>> ColorClustering::assignColorsToCenters(
    const std::map<std::string, std::vector<cv::Vec<float, 3>>>& cubeSamples)
{
    std::vector<std::string> facesInOrder = {"Yellow", "Orange", "Red", "Green", "Blue", "White"};

    std::map<std::string, cv::Vec<float, 3>> faceCenters;
    for (const auto& [face, samples] : cubeSamples) {
        faceCenters[face] = samples[4]; // center sample
    }

    std::map<std::string, std::vector<std::string>> clusteredStickers;
    for (const auto& face : facesInOrder) {
        const auto& samples = cubeSamples.at(face);
        std::vector<std::string> assigned(9);
        for (int i = 0; i < 9; ++i) {
            const auto& sample = samples[i];
            float minDistance = std::numeric_limits<float>::max();
            std::string bestFace;
            for (const auto& [centerFace, centerHSV] : faceCenters) {
                float dist = hsvDistance(sample, centerHSV);
                if (dist < minDistance) {
                    minDistance = dist;
                    bestFace = centerFace;
                }
            }
            assigned[i] = bestFace;
        }
        clusteredStickers[face] = assigned;
    }
    return clusteredStickers;
}
