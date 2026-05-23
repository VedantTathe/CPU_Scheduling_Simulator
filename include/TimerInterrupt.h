#pragma once

#include <chrono>
#include <thread>

/**
 * @class TimerInterrupt
 * @brief Simulates periodic CPU timer ticks that drive simulated process scheduling.
 */
class TimerInterrupt {
private:
    int tickIntervalMs;

public:
    /**
     * @brief Constructor.
     * @param intervalMs Real-world milliseconds corresponding to one clock cycle tick.
     */
    TimerInterrupt(int intervalMs = 200) : tickIntervalMs(intervalMs) {}
    
    void setInterval(int intervalMs) {
        tickIntervalMs = intervalMs;
    }
    
    int getInterval() const {
        return tickIntervalMs;
    }

    /**
     * @brief Sleep to simulate one CPU execution cycle.
     */
    void sleepForTick() const {
        std::this_thread::sleep_for(std::chrono::milliseconds(tickIntervalMs));
    }
};
