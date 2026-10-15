#include "color_clustering.h"
#include <limits>
#include <algorithm>
#include <vector>
#include <tuple>
#include <map>
#include <string>
#include <cmath>
#include <opencv2/opencv.hpp>

namespace {
// Lower weight for V, because brightness varies a lot between scanned faces
const float kValueWeight = 0.4f;

// HSV cone: hue becomes an angle scaled by saturation, so low-saturation (white) stickers
// ignore their noisy hue and red at H~2 is close to red at H~178
cv::Vec<float, 3> toCone(const cv::Vec<float, 3>& hsv) {
    float angle = hsv[0] * static_cast<float>(CV_PI) / 90.0f;
    return {hsv[1] * std::cos(angle), hsv[1] * std::sin(angle), hsv[2] * kValueWeight};
}

cv::Vec<float, 3> fromCone(const cv::Vec<float, 3>& cone) {
    float h = std::atan2(cone[1], cone[0]) * 90.0f / static_cast<float>(CV_PI);
    if (h < 0) h += 180.0f;
    return {h, std::hypot(cone[0], cone[1]), cone[2] / kValueWeight};
}
}

float ColorClustering::hsvDistance(const cv::Vec<float, 3>& a, const cv::Vec<float, 3>& b) {
    cv::Vec<float, 3> d = toCone(a) - toCone(b);
    return d.dot(d);
}

bool ColorClustering::compareByDistance(const std::pair<float, cv::Vec<float, 3>>& a,
    const std::pair<float, cv::Vec<float, 3>>& b) {
    return a.first < b.first;
}

std::map<std::string, std::vector<std::string>> ColorClustering::assignColorsToCenters(
    const std::map<std::string, std::vector<cv::Vec<float, 3>>>& cubeSamples)
{
    std::vector<std::string> facesInOrder = {"Yellow", "Orange", "Red", "Green", "Blue", "White"};

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

    std::map<std::string, cv::Vec<float, 3>> faceCenters;
    for (const auto& face : facesInOrder) {
        faceCenters[face] = cubeSamples.at(face)[4];
    }

    std::vector<std::string> stickerToFace(54, "");

    for (int iteration = 0; iteration < 5; ++iteration) {
        std::vector<std::tuple<float, int, std::string>> allDistances;
        for (int i = 0; i < 54; ++i) {
            for (const auto& face : facesInOrder) {
                float dist = hsvDistance(allStickers[i].hsv, faceCenters[face]);
                allDistances.emplace_back(dist, i, face);
            }
        }
        std::sort(allDistances.begin(), allDistances.end());

        std::map<std::string, int> faceCounts;
        std::vector<bool> stickerAssigned(54, false);
        std::fill(stickerToFace.begin(), stickerToFace.end(), "");

        for (int f = 0; f < 6; ++f) {
            int centerIdx = f * 9 + 4;
            stickerToFace[centerIdx] = facesInOrder[f];
            stickerAssigned[centerIdx] = true;
            faceCounts[facesInOrder[f]] = 1;
        }

        for (const auto& [dist, stickerIdx, face] : allDistances) {
            if (!stickerAssigned[stickerIdx] && faceCounts[face] < 9) {
                stickerToFace[stickerIdx] = face;
                stickerAssigned[stickerIdx] = true;
                faceCounts[face]++;
            }
        }

        // Use the mean of all 9 assigned stickers as the new reference instead of a single center sample
        std::map<std::string, cv::Vec<float, 3>> coneSums;
        for (int i = 0; i < 54; ++i) {
            coneSums[stickerToFace[i]] += toCone(allStickers[i].hsv);
        }
        for (const auto& face : facesInOrder) {
            faceCenters[face] = fromCone(coneSums[face] / 9.0f);
        }
    }

    std::map<std::string, std::vector<std::string>> clusteredStickers;
    int idx = 0;
    for (const auto& face : facesInOrder) {
        clusteredStickers[face].resize(9);
        for (int i = 0; i < 9; ++i, ++idx) {
            clusteredStickers[face][i] = stickerToFace[idx];
        }
    }

    //Rotate U (Yellow) face 90 degrees clockwise
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

    //Rotate D (White) face 180 degrees
    {
        const std::vector<std::string>& orig = clusteredStickers["White"];
        std::vector<std::string> rotated(9);
        rotated[0] = orig[8];
        rotated[1] = orig[7];
        rotated[2] = orig[6];
        rotated[3] = orig[5];
        rotated[4] = orig[4];
        rotated[5] = orig[3];
        rotated[6] = orig[2];
        rotated[7] = orig[1];
        rotated[8] = orig[0];
        clusteredStickers["White"] = rotated;
    }

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
