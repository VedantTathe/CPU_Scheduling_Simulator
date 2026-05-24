#pragma once

#include "CPUProcess.h"
#include <vector>
#include <string>

/**
 * @class AdaptiveSchedulerAnalyzer
 * @brief Analyzes a given set of processes dynamically and recommends the best scheduling algorithm.
 * Supports both an online mode (DeepSeek AI endpoint via curl) and an offline mode (mathematical rule engine).
 */
class AdaptiveSchedulerAnalyzer {
public:
    AdaptiveSchedulerAnalyzer() = default;
    ~AdaptiveSchedulerAnalyzer() = default;

    /**
     * @brief Analyzes loaded processes and determines the absolute best algorithm.
     * @param processes List of active processes loaded in the simulator runtime.
     * @param explanationOut [Output] Fully explained reasoning text detailing selection metrics.
     * @return Unique name of recommended scheduler (e.g., "SJF", "Priority", "EDF", "FCFS", "RR").
     */
    std::string analyzeAndSelect(const std::vector<Process>& processes, std::string& explanationOut);

private:
    /**
     * @brief Analyzes processes offline using strict mathematical, statistical, and state properties.
     */
    std::string selectOfflineHeuristic(const std::vector<Process>& processes, std::string& explanationOut);

    /**
     * @brief Queries the DeepSeek Chat API using a system-level curl pipeline (dependency-free).
     */
    std::string selectOnlineAI(const std::vector<Process>& processes, const std::string& apiKey, std::string& explanationOut);

    /**
     * @brief Extracts standard deviation of burst times from the process set.
     */
    double calculateBurstStdDev(const std::vector<Process>& processes) const;

    /**
     * @brief Checks if the registered schedulers support a given algorithm.
     */
    bool isAlgorithmRegistered(const std::string& name) const;
};
