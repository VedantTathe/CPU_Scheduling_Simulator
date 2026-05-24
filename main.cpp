#include "SimulatorUI.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

#ifdef _WIN32
#include <stdlib.h>
#else
#include <unistd.h>
#endif

#include <vector>

#ifdef _WIN32
#include <windows.h>
std::string getExeDir() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string pos(buffer);
    size_t lastSlash = pos.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        return pos.substr(0, lastSlash);
    }
    return "";
}
#else
#include <limits.h>
#include <unistd.h>
std::string getExeDir() {
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        std::string pos(buffer);
        size_t lastSlash = pos.find_last_of("/");
        if (lastSlash != std::string::npos) {
            return pos.substr(0, lastSlash);
        }
    }
    return "";
}
#endif

void loadEnvFile() {
    std::vector<std::string> paths = {
        ".env",
        "../.env",
        "../../.env",
        "../../../.env"
    };

    std::string exeDir = getExeDir();
    if (!exeDir.empty()) {
        paths.push_back(exeDir + "/.env");
        paths.push_back(exeDir + "/../.env");
        paths.push_back(exeDir + "/../../.env");
        paths.push_back(exeDir + "/../../../.env");
    }

    for (const auto& path : paths) {
        std::ifstream file(path);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                // Trim whitespace
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                line.erase(line.find_last_not_of(" \t\r\n") + 1);

                if (line.empty() || line[0] == '#') {
                    continue;
                }

                size_t eqPos = line.find('=');
                if (eqPos == std::string::npos) {
                    continue;
                }

                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);

                // Trim key and value
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);

                if (!key.empty()) {
#ifdef _WIN32
                    _putenv_s(key.c_str(), value.c_str());
#else
                    setenv(key.c_str(), value.c_str(), 1);
#endif
                }
            }
            break; // Stop at first successfully loaded .env file
        }
    }
}

int main() {
#ifdef _WIN32
    // Set console output and input code page to UTF-8 (65001) to ensure proper rendering of unicode box-drawing characters
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    loadEnvFile();
    SimulatorUI simulator;
    simulator.run();
    return 0;
}

