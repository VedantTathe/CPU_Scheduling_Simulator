#pragma once

#include "Process.h"
#include "Metrics.h"
#include <vector>
#include <string>

// Console color codes (optional ANSI coloring)
namespace ConsoleColor {
    constexpr const char* RESET   = "\033[0m";
    constexpr const char* BOLD    = "\033[1m";
    constexpr const char* GREEN   = "\033[32m";
    constexpr const char* YELLOW  = "\033[33m";
    constexpr const char* CYAN    = "\033[36m";
    constexpr const char* MAGENTA = "\033[35m";
    constexpr const char* HEADER  = "\033[36m";
}

/**
 * @namespace Utils
 * @brief Utility functions for the CPU Scheduling Simulator with enhanced visualization.
 */
namespace Utils {

    /**
     * @brief Print a horizontal divider line.
     * @param width Width of the divider line
     * @param c Character to use for the line
     */
    void printDivider(int width = 80, char c = '-');

    /**
     * @brief Print a formatted header with visual styling.
     * @param title Title of the section
     */
    void printHeader(const std::string& title);

    /**
     * @brief Print a section header (lighter formatting).
     * @param title Title of the subsection
     */
    void printSectionHeader(const std::string& title);

    /**
     * @brief Print processes in a formatted table.
     * @param processes Vector of Process objects
     */
    void printProcessTable(const std::vector<Process>& processes);

    /**
     * @brief Print scheduling metrics in a formatted manner.
     * @param metrics Metrics object containing calculated values
     */
    void printMetrics(const Metrics& metrics);

    /**
     * @brief Sort processes by arrival time.
     * @param processes Vector to be sorted (modified in-place)
     */
    void sortByArrivalTime(std::vector<Process>& processes);

    /**
     * @brief Sort processes by burst time.
     * @param processes Vector to be sorted (modified in-place)
     */
    void sortByBurstTime(std::vector<Process>& processes);

    /**
     * @brief Sort processes by priority.
     * @param processes Vector to be sorted (modified in-place)
     */
    void sortByPriority(std::vector<Process>& processes);

    /**
     * @brief Format time value for display.
     * @param time Time value in milliseconds/units
     * @return Formatted string representation
     */
    std::string formatTime(int time);

    /**
     * @brief Print an enhanced Gantt chart in professional ASCII format.
     * @param processes Vector of completed processes
     * @param totalTime Total execution time
     */
    void printGanttChart(const std::vector<Process>& processes, int totalTime);

    /**
     * @brief Print professional Gantt chart with improved formatting and idle visualization.
     * @param processes Vector of completed processes in execution order
     * @param totalTime Total execution time
     */
    void printEnhancedGanttChart(const std::vector<Process>& processes, int totalTime);

    /**
     * @brief Print a success message with formatting.
     * @param message Message text
     */
    void printSuccess(const std::string& message);

    /**
     * @brief Print a warning message with formatting.
     * @param message Message text
     */
    void printWarning(const std::string& message);

    /**
     * @brief Print information with formatting.
     * @param message Message text
     */
    void printInfo(const std::string& message);

    /**
     * @brief Convert ProcessState enum value to a human-readable string.
     * @param state The ProcessState value
     * @return String representation of the state
     */
    std::string toString(ProcessState state);

    /**
     * @brief Print a process state transition with timestamp and pid.
     * @param time The simulation time at transition
     * @param pid The process ID
     * @param oldState The state before transition
     * @param newState The state after transition
     */
    void printStateTransition(int time, int pid, ProcessState oldState, ProcessState newState);

} // namespace Utils
