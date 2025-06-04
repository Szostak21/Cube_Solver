#include "kociemba_string.h"
#include <vector>
#include <map>
#include <string>

std::string buildKociembaString(const std::map<std::string, std::vector<std::string>>& clusteredStickers) {
    std::map<std::string, char> colorToChar = {
        {"Yellow", 'U'},
        {"Green", 'L'},
        {"Orange", 'F'},
        {"Blue", 'R'},
        {"Red", 'B'},
        {"White", 'D'}
    };

    // Kociemba face order: U R F D L B
    std::vector<std::string> kociembaFaces = {"Yellow", "Blue", "Orange", "White", "Green", "Red"};
    std::string kociembaString;

    for (const auto& face : kociembaFaces) {
        const auto& stickers = clusteredStickers.at(face);
        for (const auto& sticker : stickers) {
            kociembaString += colorToChar[sticker];
        }
    }
    return kociembaString;
}