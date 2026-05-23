#pragma once

#include "CPUProcess.h"
#include "CPUCore.h"
#include <vector>
#include <string>
#include <memory>
#include <mutex>

/**
 * @class DashboardRenderer
 * @brief Renders a beautiful real-time monitor dashboard in the terminal (htop-style).
 */
class DashboardRenderer {
private:
    std::vector<std::string> eventLog;
    const size_t maxEvents = 8;
    mutable std::mutex logMutex;
    
public:
    DashboardRenderer();
    ~DashboardRenderer() = default;

    /**
     * @brief Initialize terminal (e.g. enabling virtual terminal processing on Windows).
     */
    void initialize();

    /**
     * @brief Add a system event to the rolling log.
     */
    void addEvent(int time, const std::string& eventText);

    /**
     * @brief Render the real-time monitoring dashboard.
     */
    void render(int currentTime, const std::string& algoName, int numCores,
                const std::vector<std::unique_ptr<CPUCore>>& cores,
                const std::vector<Process>& processes);

    /**
     * @brief Reset/Clear the screen to home position (flicker-free).
     */
    void clearScreen() const;
};
