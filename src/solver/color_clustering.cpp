#include "color_clustering.h"
#include <limits>
#include <algorithm>
#include <vector>
#include <tuple>
#include <map>
#include <string>
#include <cmath>
#include <opencv2/opencv.hpp>

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

    // 1. Gather all stickers (face, idx, hsv)
    struct Sticker {
        std::string face;
        int idx;
        cv::Vec<float, 3> hsv;
    };
    std::vector<Sticker> allStickers;
    for (const auto& face : facesInOrder) {
        const auto& samples = cubeSamples.at(face);
        for (int i = 0; i < 9; ++i) {
            allStickers.push_back({face, i, samples[i]});
        }
    }

    // 2. Get center HSVs
    std::map<std::string, cv::Vec<float, 3>> faceCenters;
    for (const auto& face : facesInOrder) {
        faceCenters[face] = cubeSamples.at(face)[4];
    }

    // 3. Build all possible assignments: (distance, stickerIdx, face)
    std::vector<std::tuple<float, int, std::string>> allDistances;
    for (int i = 0; i < 54; ++i) {
        for (const auto& face : facesInOrder) {
            float dist = hsvDistance(allStickers[i].hsv, faceCenters[face]);
            allDistances.emplace_back(dist, i, face);
        }
    }
    std::sort(allDistances.begin(), allDistances.end());

    // 4. Greedy global assignment: each sticker to one face, each face gets 9 stickers
    std::map<std::string, int> faceCounts;
    std::vector<std::string> stickerToFace(54, "");
    std::vector<bool> stickerAssigned(54, false);

    for (const auto& [dist, stickerIdx, face] : allDistances) {
        if (!stickerAssigned[stickerIdx] && faceCounts[face] < 9) {
            stickerToFace[stickerIdx] = face;
            stickerAssigned[stickerIdx] = true;
            faceCounts[face]++;
        }
    }

    // 5. Build output: for each face, vector of 9 assigned face names (in original order)
    std::map<std::string, std::vector<std::string>> clusteredStickers;
    int idx = 0;
    for (const auto& face : facesInOrder) {
        clusteredStickers[face].resize(9);
        for (int i = 0; i < 9; ++i, ++idx) {
            clusteredStickers[face][i] = stickerToFace[idx];
        }
    }

    // 6. Rotate U (Yellow) face 90 degrees clockwise if needed
    {
        const std::vector<std::string>& orig = clusteredStickers["Yellow"];
        std::vector<std::string> rotated(9);
        rotated[0] = orig[6];
        rotated[1] = orig[3];
        rotated[2] = orig[0];
        rotated[3] = orig[7];
        rotated[4] = orig[4];
        rotated[5] = orig[1];
        rotated[6] = orig[8];
        rotated[7] = orig[5];
        rotated[8] = orig[2];
        clusteredStickers["Yellow"] = rotated;
    }

    // 7. Final check: ensure exactly 9 of each face label
    std::map<std::string, int> colorCounts;
    for (const auto& [face, stickers] : clusteredStickers) {
        for (const auto& sticker : stickers) {
            colorCounts[sticker]++;
        }
    }
    for (const auto& face : facesInOrder) {
        if (colorCounts[face] != 9) {
            std::cerr << "Warning: Face " << face << " has " << colorCounts[face] << " stickers assigned!" << std::endl;
        }
    }

    return clusteredStickers;
}
