#pragma once

#include "CPUProcess.h"

/**
 * @class ContextSwitch
 * @brief Utility class to simulate and visualize operating system context switches.
 * 
 * Provides a standardized way to log context saving and restoring/loading,
 * and optional delay simulation using real-time sleeping.
 */
class ContextSwitch {
public:
    /**
     * @brief Perform and visualize a context switch.
     * 
     * Prints the saving of the current process context, restoring/loading of
     * the next process context, and completion logs. Sleep delay is applied
     * if realTimeDelayMs > 0.
     * 
     * @param currentTime Current simulation time
     * @param outgoing Pointer to the process leaving the CPU (nullptr if CPU was idle)
     * @param incoming Pointer to the process entering the CPU (nullptr if CPU is going idle)
     * @param realTimeDelayMs Real-world pause duration in milliseconds
     */
    static void performSwitch(int currentTime, const Process* outgoing, const Process* incoming, int realTimeDelayMs);
};
