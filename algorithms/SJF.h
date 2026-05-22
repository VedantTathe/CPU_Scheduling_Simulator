#pragma once

#include "../include/Scheduler.h"

/**
 * @class SJFScheduler
 * @brief Shortest Job First (SJF) CPU scheduling algorithm implementation.
 * 
 * SJF is a non-preemptive scheduling algorithm where:
 * - Among all arrived processes, the one with shortest burst time executes next
 * - Reduces average waiting time compared to FCFS
 * - Can suffer from starvation (long jobs may wait indefinitely)
 * 
 * Scheduling Logic:
 * 1. Among all processes that have arrived, select the one with smallest burst
 * 2. If no process has arrived, advance time to next arrival
 * 3. Execute selected process to completion
 * 4. Repeat until all processes complete
 * 
 * Time Complexity: O(n²) - scans process list for each scheduling decision
 * Can be optimized with priority queue/heap to O(n log n)
 */
class SJFScheduler : public Scheduler {
public:
    /**
     * @brief Constructor
     */
    SJFScheduler();

    /**
     * @brief Execute SJF scheduling algorithm.
     * 
     * Dynamically selects process with shortest burst among available processes.
     * Key difference from FCFS: not bound to arrival order.
     */
    void run() override;

    /**
     * @brief Calculate performance metrics for SJF scheduling.
     */
    void calculateMetrics() override;

    /**
     * @brief Display scheduling results with Gantt chart and metrics.
     */
    void displayResults() const override;

    /**
     * @brief Get the algorithm name.
     * @return "Shortest Job First (SJF)"
     */
    std::string getAlgorithmName() const override;

private:
    /**
     * @brief Find next process to schedule among arrived processes.
     * 
     * Selection criteria:
     * - Process has arrived (arrivalTime <= currentTime)
     * - Process not yet scheduled (remainingTime > 0 or just checking if scheduled)
     * - Has minimum burst time among arrived unscheduled processes
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
