#pragma once

#include "../include/Scheduler.h"
#include <vector>
#include <string>

/**
 * @class AlgorithmComparison
 * @brief Framework for comparing multiple scheduling algorithms.
 * 
 * This class enables side-by-side comparison of scheduling algorithm performance.
 * Useful for understanding trade-offs and evaluating algorithm effectiveness.
 */
class AlgorithmComparison {
public:
    /**
     * @struct AlgorithmResult
     * @brief Stores results of a single algorithm execution.
     */
    struct AlgorithmResult {
        std::string algorithmName;
        Metrics metrics;
        int totalExecutionTime;
    };

    /**
     * @brief Add a scheduler for comparison.
     * @param scheduler Scheduler to test
     * @param processes Vector of processes to use
     */
    void addScheduler(Scheduler* scheduler, const std::vector<Process>& processes);

    /**
     * @brief Run all schedulers and collect results.
     */
    void runAllSchedulers();

    /**
     * @brief Display comparison results in formatted table.
     */
    void displayComparison() const;

    /**
     * @brief Get results for specific algorithm.
     * @param algorithmName Name of algorithm
     * @return AlgorithmResult with metrics
     */
    const AlgorithmResult* getResult(const std::string& algorithmName) const;

    /**
     * @brief Clear all results.
     */
    void clear();

private:
    struct SchedulerInfo {
        Scheduler* scheduler;
        std::vector<Process> processes;
    };

    std::vector<SchedulerInfo> schedulers;
    std::vector<AlgorithmResult> results;

    /**
     * @brief Print comparison header.
     */
    void printComparisonHeader() const;

    /**
     * @brief Print comparison table.
     */
    void printComparisonTable() const;

    /**
     * @brief Determine which algorithm is better for each metric.
     * @param metric Current metric name
     * @param value Current value
     * @return true if "lower is better", false if "higher is better"
     */
    bool isLowerBetter(const std::string& metric) const;
};
