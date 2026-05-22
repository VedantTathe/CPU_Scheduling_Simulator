#pragma once

#include "../include/Scheduler.h"
#include <vector>

/**
 * @class FCFSScheduler
 * @brief First Come First Served (FCFS) CPU scheduling algorithm implementation.
 * 
 * FCFS is a non-preemptive scheduling algorithm where:
 * - Processes are executed in the order they arrive
 * - Once a process starts, it runs to completion
 * - Simple to implement and understand
 * - Can suffer from convoy effect (long process blocks others)
 * 
 * Scheduling Logic:
 * 1. Sort processes by arrival time
 * 2. Pick the first available process
 * 3. Run it completely (non-preemptive)
 * 4. Move to next process
 */
class FCFSScheduler : public Scheduler {
public:
    /**
     * @brief Constructor
     */
    FCFSScheduler();

    /**
     * @brief Execute FCFS scheduling algorithm.
     * 
     * This method:
     * - Sorts processes by arrival time
     * - Simulates CPU execution timeline
     * - Calculates timing metrics for each process
     */
    void run() override;

    /**
     * @brief Calculate performance metrics.
     * 
     * Computes:
     * - Average waiting time
     * - Average turnaround time
     * - Average response time
     * - CPU utilization
     * - Throughput
     */
    void calculateMetrics() override;

    /**
     * @brief Display scheduling results.
     * 
     * Shows:
     * - Process execution table
     * - Gantt chart
     * - Performance metrics
     */
    void displayResults() const override;

    /**
     * @brief Get the algorithm name.
     * @return "First Come First Served (FCFS)"
     */
    std::string getAlgorithmName() const override;

private:
    /**
     * @brief Draw a Gantt chart for visualization.
     */
    void drawGanttChart() const;

    /**
     * @brief Print a detailed process table.
     */
    void printProcessTable() const;

    /**
     * @brief Print performance metrics summary.
     */
    void printMetricsSummary() const;

    /**
     * @brief Helper: Calculate response time for a process.
     * In FCFS, response time = waiting time.
     * @param process The process to calculate for
     * @return Response time in time units
     */
    int calculateResponseTime(const Process& process) const;
};
