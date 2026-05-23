#pragma once

#include "../include/Scheduler.h"
#include <vector>
#include <string>
#include <map>

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
     * @struct AlgorithmRank
     * @brief Ranking information for an algorithm across multiple metrics.
     */
    struct AlgorithmRank {
        std::string algorithmName;
        int overallScore;
        std::map<std::string, int> metricRanks;
        int totalMetricsWon;
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
     * @brief Display professional comparison dashboard with analysis.
     */
    void displayProfessionalDashboard() const;

    /**
     * @brief Get algorithm rankings based on performance.
     * @return Vector of AlgorithmRank sorted by overall score
     */
    std::vector<AlgorithmRank> getRankings() const;

    /**
     * @brief Get best algorithm recommendation with explanation.
     * @return Pair of (algorithm name, explanation)
     */
    std::pair<std::string, std::string> getRecommendation() const;

    /**
     * @brief Get a detailed AI-based analysis explanation of comparison results.
     * Generates dynamic online Gemini AI analysis if API key is present, otherwise falls back to a smart offline C++ engine.
     */
    std::string getAIExplanation() const;

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

    /**
     * @brief Calculate rankings for all algorithms based on performance.
     * @return Vector of AlgorithmRank sorted by overall score
     */
    std::vector<AlgorithmRank> calculateRankings() const;

    /**
     * @brief Generate best algorithm recommendation with explanation.
     * @return Pair of (algorithm name, explanation string)
     */
    std::pair<std::string, std::string> generateRecommendation() const;

    /**
     * @brief Generates an offline C++ rule-based explanation of metrics and tradeoffs.
     */
    std::string generateOfflineAIExplanation() const;

    /**
     * @brief Queries the Gemini API for results explanation using a system curl process.
     */
    std::string queryGeminiAIExplanation(const std::string& prompt, const std::string& apiKey) const;

    /**
     * @brief Print professional dashboard header and title.
     */
    void printDashboardHeader() const;

    /**
     * @brief Print formatted metrics comparison table for dashboard.
     */
    void printMetricsTable() const;

    /**
     * @brief Print algorithm rankings section.
     */
    void printRankingsSection() const;

    /**
     * @brief Print recommendation section.
     */
    void printRecommendationSection() const;

    /**
     * @brief Get best performing algorithm for a specific metric.
     * @param metric The metric name (e.g., "Waiting Time")
     * @return Name of best algorithm for that metric
     */
    std::string getBestAlgorithmForMetric(const std::string& metric) const;

    /**
     * @brief Format metric value for display with precision.
     * @param value The metric value
     * @param precision Decimal places to show
     * @return Formatted string representation
     */
    std::string formatMetricValue(double value, int precision = 2) const;
};
