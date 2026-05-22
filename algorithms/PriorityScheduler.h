#pragma once

#include "../include/Scheduler.h"

/**
 * @class PriorityScheduler
 * @brief Non-preemptive Priority CPU scheduling algorithm implementation.
 * 
 * Priority Scheduling is a non-preemptive algorithm where:
 * - Among all arrived processes, the one with HIGHEST priority executes next
 * - Lower numeric priority value = HIGHER scheduling priority
 * - Simple to understand and implement
 * - Suffers from starvation (low-priority processes may wait indefinitely)
 * 
 * Priority Levels:
 * - Priority 1 = Highest (executes first)
 * - Priority 2 = Medium
 * - Priority 3, 4, ... = Lower priorities
 * 
 * Scheduling Logic:
 * 1. Among all processes that have arrived, select the one with LOWEST priority number
 * 2. If no process has arrived, advance time to next arrival
 * 3. Execute selected process to completion
 * 4. Repeat until all processes complete
 * 
 * Time Complexity: O(n²) - scans process list for each scheduling decision
 * Can be optimized with priority queue/heap to O(n log n)
 * 
 * Starvation Problem:
 * - If high-priority processes keep arriving, low-priority processes never run
 * - Solution: Aging (increase priority of waiting processes over time) - future enhancement
 */
class PriorityScheduler : public Scheduler {
public:
    /**
     * @brief Constructor
     */
    PriorityScheduler();

    /**
     * @brief Execute Priority scheduling algorithm.
     * 
     * Dynamically selects process with highest priority among available processes.
     * Lower numeric priority value = higher scheduling priority.
     */
    void run() override;

    /**
     * @brief Calculate performance metrics for Priority scheduling.
     */
    void calculateMetrics() override;

    /**
     * @brief Display scheduling results with Gantt chart and metrics.
     */
    void displayResults() const override;

    /**
     * @brief Get the algorithm name.
     * @return "Priority Scheduling"
     */
    std::string getAlgorithmName() const override;

private:
    /**
     * @brief Find next process to schedule among arrived processes.
     * 
     * Selection criteria:
     * - Process has arrived (arrivalTime <= currentTime)
     * - Process not yet scheduled
     * - Has HIGHEST priority (LOWEST numeric value) among arrived unscheduled processes
     * 
     * @param currentTime Current simulation time
     * @param scheduled Vector of already scheduled process IDs
     * @return Pointer to selected Process, or nullptr if none available
     */
    Process* selectNextProcess(int currentTime, const std::vector<int>& scheduled);

    /**
     * @brief Draw Gantt chart visualization.
     */
    void drawGanttChart() const;

    /**
     * @brief Print process execution table.
     */
    void printProcessTable() const;

    /**
     * @brief Print metrics summary.
     */
    void printMetricsSummary() const;
};
