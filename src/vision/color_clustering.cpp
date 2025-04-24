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

std::map<std::string, std::vector<cv::Vec<float, 3>>> ColorClustering::assignColorsToCenters(const std::map<std::string, std::vector<cv::Vec<float, 3>>>& cubeSamples) {
    std::map<std::string, std::vector<cv::Vec<float, 3>>> clustered;
    std::map<std::string, cv::Vec<float, 3>> faceCenters;

    for (auto& [face, samples] : cubeSamples) {
        faceCenters[face] = samples[4];
    }

    for (auto& [face, samples] : cubeSamples) {
        std::vector<std::pair<float, cv::Vec<float, 3>>> distances;

        for (auto& sample : samples) {
            float distance = hsvDistance(sample, faceCenters[face]);
            distances.push_back({distance, sample});
        }

        std::sort(distances.begin(), distances.end(), compareByDistance);

        std::vector<cv::Vec<float, 3>> closestSamples;
        for (int i = 0; i < 9; ++i) {
            closestSamples.push_back(distances[i].second);
        }

        clustered[face] = closestSamples;
    }

    return clustered;
}
