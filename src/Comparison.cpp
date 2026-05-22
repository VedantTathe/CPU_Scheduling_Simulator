#include "Comparison.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

void AlgorithmComparison::addScheduler(Scheduler* scheduler, const std::vector<Process>& processes) {
    schedulers.push_back({scheduler, processes});
}

void AlgorithmComparison::runAllSchedulers() {
    results.clear();

    for (auto& info : schedulers) {
        // Clear any previous state
        info.scheduler->clearProcesses();

        // Add all processes
        for (const auto& p : info.processes) {
            info.scheduler->addProcess(p);
        }

        // Run the scheduler
        info.scheduler->run();
        info.scheduler->calculateMetrics();

        // Store results
        AlgorithmResult result;
        result.algorithmName = info.scheduler->getAlgorithmName();
        result.metrics = info.scheduler->getMetrics();
        result.totalExecutionTime = info.scheduler->getProcessCount() > 0 
            ? info.processes[0].completionTime 
            : 0;

        // Get actual total execution time from processes
        int maxCompletion = 0;
        for (const auto& p : info.processes) {
            maxCompletion = std::max(maxCompletion, p.completionTime);
        }
        result.totalExecutionTime = maxCompletion;

        results.push_back(result);
    }
}

void AlgorithmComparison::displayComparison() const {
    Utils::printHeader("Algorithm Comparison Results");
    printComparisonTable();
}

const AlgorithmComparison::AlgorithmResult* AlgorithmComparison::getResult(const std::string& algorithmName) const {
    for (const auto& result : results) {
        if (result.algorithmName == algorithmName) {
            return &result;
        }
    }
    return nullptr;
}

void AlgorithmComparison::clear() {
    schedulers.clear();
    results.clear();
}

void AlgorithmComparison::printComparisonHeader() const {
    std::cout << std::left
              << std::setw(35) << "Algorithm"
              << std::setw(20) << "Avg Wait Time"
              << std::setw(20) << "Avg Turnaround"
              << std::setw(18) << "Avg Response"
              << std::setw(15) << "Total Time"
              << std::endl;
    Utils::printDivider(100, '-');
}

void AlgorithmComparison::printComparisonTable() const {
    if (results.empty()) {
        std::cout << "No results to display. Run schedulers first." << std::endl;
        return;
    }

    printComparisonHeader();

    std::cout << std::fixed << std::setprecision(2);

    for (const auto& result : results) {
        std::cout << std::left
                  << std::setw(35) << result.algorithmName
                  << std::setw(20) << result.metrics.averageWaitingTime
                  << std::setw(20) << result.metrics.averageTurnaroundTime
                  << std::setw(18) << result.metrics.averageResponseTime
                  << std::setw(15) << result.totalExecutionTime
                  << std::endl;
    }

    std::cout << "\n";

    // Display analysis
    std::cout << "Analysis:" << std::endl;
    if (results.size() >= 2) {
        double fcfsWait = results[0].metrics.averageWaitingTime;
        double sjfWait = results[1].metrics.averageWaitingTime;

        if (sjfWait < fcfsWait) {
            std::cout << "  [+] SJF has " << std::fixed << std::setprecision(2) 
                      << ((fcfsWait - sjfWait) / fcfsWait * 100.0) 
                      << "% lower average waiting time than FCFS" << std::endl;
        }

        double fcfsTurnaround = results[0].metrics.averageTurnaroundTime;
        double sjfTurnaround = results[1].metrics.averageTurnaroundTime;

        if (sjfTurnaround < fcfsTurnaround) {
            std::cout << "  [+] SJF has " << std::fixed << std::setprecision(2) 
                      << ((fcfsTurnaround - sjfTurnaround) / fcfsTurnaround * 100.0) 
                      << "% lower average turnaround time than FCFS" << std::endl;
        }
    }
}

bool AlgorithmComparison::isLowerBetter(const std::string& metric) const {
    // For scheduling metrics, lower waiting/turnaround time is better
    return true;
}

// ============================================================================
// PROFESSIONAL DASHBOARD IMPLEMENTATION
// ============================================================================

void AlgorithmComparison::displayProfessionalDashboard() const {
    if (results.empty()) {
        std::cout << "No results to display. Run schedulers first." << std::endl;
        return;
    }

    printDashboardHeader();
    std::cout << std::endl;

    // Print metrics table
    printMetricsTable();
    std::cout << std::endl;

    // Print rankings section
    printRankingsSection();
    std::cout << std::endl;

    // Print recommendation section
    printRecommendationSection();
}

std::vector<AlgorithmComparison::AlgorithmRank> AlgorithmComparison::getRankings() const {
    return calculateRankings();
}

std::pair<std::string, std::string> AlgorithmComparison::getRecommendation() const {
    return generateRecommendation();
}

void AlgorithmComparison::printDashboardHeader() const {
    Utils::printHeader("PROFESSIONAL ANALYSIS DASHBOARD");
}

void AlgorithmComparison::printMetricsTable() const {
    Utils::printHeader("Performance Metrics Comparison");

    // Print table header
    std::cout << std::left
              << std::setw(25) << "Metric"
              << std::setw(20) << "FCFS"
              << std::setw(20) << "SJF"
              << std::setw(20) << "Priority"
              << std::setw(15) << "Best"
              << std::endl;
    Utils::printDivider(100, '-');

    std::cout << std::fixed << std::setprecision(2);

    // Metrics to display
    std::vector<std::pair<std::string, double (Metrics::*)>> metrics = {
        {"Avg Waiting Time", &Metrics::averageWaitingTime},
        {"Avg Turnaround Time", &Metrics::averageTurnaroundTime},
        {"Avg Response Time", &Metrics::averageResponseTime},
        {"CPU Utilization %", &Metrics::cpuUtilization},
        {"Throughput", &Metrics::throughput}
    };

    for (const auto& metric : metrics) {
        std::cout << std::left << std::setw(25) << metric.first;

        // Print values for each algorithm
        std::string bestAlgo = getBestAlgorithmForMetric(metric.first);

        for (const auto& result : results) {
            double value = result.metrics.*metric.second;
            std::cout << std::setw(20) << formatMetricValue(value, 2);
        }

        // Print best algorithm for this metric
        std::cout << std::setw(15) << ("(" + bestAlgo + ")");
        std::cout << std::endl;
    }

    std::cout << std::endl;
}

void AlgorithmComparison::printRankingsSection() const {
    Utils::printHeader("Algorithm Rankings");

    auto rankings = calculateRankings();

    std::cout << std::left
              << std::setw(5) << "Rank"
              << std::setw(20) << "Algorithm"
              << std::setw(15) << "Score"
              << std::setw(20) << "Metrics Won"
              << std::endl;
    Utils::printDivider(60, '-');

    int rank = 1;
    for (const auto& result : rankings) {
        std::cout << std::left
                  << std::setw(5) << (std::to_string(rank) + ".")
                  << std::setw(20) << result.algorithmName
                  << std::setw(15) << result.overallScore
                  << std::setw(20) << result.totalMetricsWon
                  << std::endl;
        rank++;
    }

    std::cout << std::endl;
}

void AlgorithmComparison::printRecommendationSection() const {
    Utils::printHeader("Recommendation");

    auto [bestAlgo, explanation] = generateRecommendation();

    std::cout << "[BEST] Best Overall Algorithm: " << bestAlgo << std::endl;
    std::cout << std::endl;
    std::cout << "Reason:" << std::endl;
    std::cout << explanation;
    std::cout << std::endl;
}

std::vector<AlgorithmComparison::AlgorithmRank> AlgorithmComparison::calculateRankings() const {
    std::vector<AlgorithmRank> rankings;

    for (const auto& result : results) {
        AlgorithmRank rank;
        rank.algorithmName = result.algorithmName;
        rank.overallScore = 0;
        rank.totalMetricsWon = 0;

        rankings.push_back(rank);
    }

    // Evaluate each metric
    std::vector<std::string> metrics = {
        "Avg Waiting Time",
        "Avg Turnaround Time",
        "Avg Response Time",
        "CPU Utilization %",
        "Throughput"
    };

    for (const auto& metric : metrics) {
        std::string bestAlgo = getBestAlgorithmForMetric(metric);

        // Find which result corresponds to best algorithm
        for (auto& rank : rankings) {
            if (rank.algorithmName == bestAlgo) {
                rank.overallScore++;
                rank.totalMetricsWon++;
                rank.metricRanks[metric] = 1;
            } else {
                // Other algorithms get secondary ranks (simplified)
                rank.metricRanks[metric] = 2;
            }
        }
    }

    // Sort by overall score (descending)
    std::sort(rankings.begin(), rankings.end(),
              [](const AlgorithmRank& a, const AlgorithmRank& b) {
                  return a.overallScore > b.overallScore;
              });

    return rankings;
}

std::pair<std::string, std::string> AlgorithmComparison::generateRecommendation() const {
    auto rankings = calculateRankings();

    if (rankings.empty()) {
        return {"None", "No algorithms to recommend.\n"};
    }

    // Get best algorithm
    const auto& best = rankings[0];

    // Build explanation
    std::string explanation;
    explanation += "  - Best performance in " + std::to_string(best.totalMetricsWon) + " metrics\n";
    explanation += "  - Superior algorithm for this workload\n";

    // Add specific strengths
    if (best.algorithmName == "First Come First Served (FCFS)") {
        explanation += "  - Simple, predictable scheduling\n";
        explanation += "  - Good for sequential, long-running tasks\n";
    } else if (best.algorithmName == "Shortest Job First (SJF)") {
        explanation += "  - Minimizes waiting time\n";
        explanation += "  - Excellent for varying burst times\n";
        explanation += "  - Best overall fairness and responsiveness\n";
    } else if (best.algorithmName == "Priority Scheduling") {
        explanation += "  - Respects priority requirements\n";
        explanation += "  - Best for mixed-priority workloads\n";
    }

    return {best.algorithmName, explanation};
}

std::string AlgorithmComparison::getBestAlgorithmForMetric(const std::string& metric) const {
    if (results.empty()) {
        return "N/A";
    }

    bool lowerIsBetter = true;  // For all scheduling metrics except throughput

    if (metric == "Throughput") {
        lowerIsBetter = false;  // Higher throughput is better
    }

    std::string bestAlgo = results[0].algorithmName;
    double bestValue = 0.0;

    // Helper lambda to get metric value
    auto getMetricValue = [&](const AlgorithmResult& result) -> double {
        if (metric == "Avg Waiting Time") return result.metrics.averageWaitingTime;
        if (metric == "Avg Turnaround Time") return result.metrics.averageTurnaroundTime;
        if (metric == "Avg Response Time") return result.metrics.averageResponseTime;
        if (metric == "CPU Utilization %") return result.metrics.cpuUtilization;
        if (metric == "Throughput") return result.metrics.throughput;
        return 0.0;
    };

    bestValue = getMetricValue(results[0]);

    for (size_t i = 1; i < results.size(); ++i) {
        double currentValue = getMetricValue(results[i]);

        bool isBetter = false;
        if (lowerIsBetter && currentValue < bestValue) {
            isBetter = true;
        } else if (!lowerIsBetter && currentValue > bestValue) {
            isBetter = true;
        }

        if (isBetter) {
            bestValue = currentValue;
            bestAlgo = results[i].algorithmName;
        }
    }

    return bestAlgo;
}

std::string AlgorithmComparison::formatMetricValue(double value, int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}
