#pragma once

#include "Process.h"
#include "Metrics.h"
#include <vector>
#include <string>

/**
 * @namespace Utils
 * @brief Utility functions for the CPU Scheduling Simulator.
 */
namespace Utils {

    /**
     * @brief Print a horizontal divider line.
     * @param width Width of the divider line
     * @param char Character to use for the line
     */
    void printDivider(int width = 80, char c = '-');

    /**
     * @brief Print a formatted header.
     * @param title Title of the section
     */
    void printHeader(const std::string& title);

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
     * @brief Print a Gantt chart in text format.
     * @param processes Vector of completed processes
     * @param totalTime Total execution time
     */
    void printGanttChart(const std::vector<Process>& processes, int totalTime);

} // namespace Utils
