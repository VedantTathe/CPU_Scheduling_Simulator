#include "Comparison.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

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
            std::cout << "  ✓ SJF has " << std::fixed << std::setprecision(2) 
                      << ((fcfsWait - sjfWait) / fcfsWait * 100.0) 
                      << "% lower average waiting time than FCFS" << std::endl;
        }

        double fcfsTurnaround = results[0].metrics.averageTurnaroundTime;
        double sjfTurnaround = results[1].metrics.averageTurnaroundTime;

        if (sjfTurnaround < fcfsTurnaround) {
            std::cout << "  ✓ SJF has " << std::fixed << std::setprecision(2) 
                      << ((fcfsTurnaround - sjfTurnaround) / fcfsTurnaround * 100.0) 
                      << "% lower average turnaround time than FCFS" << std::endl;
        }
    }
}

bool AlgorithmComparison::isLowerBetter(const std::string& metric) const {
    // For scheduling metrics, lower waiting/turnaround time is better
    return true;
}
