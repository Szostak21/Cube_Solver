#include "kociemba.h"

std::string run_python_kociemba(const std::string& kociembaString) {
    std::string command = "python3 -c \"import kociemba; print(kociemba.solve('" + kociembaString + "'))\" 2>/dev/null";
    std::array<char, 128> buffer;
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);
    if (!result.empty() && result[result.size()-1] == '\n') result.pop_back();
    return result;
}