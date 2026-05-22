#include "FCFS.h"
#include "../include/Utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

FCFSScheduler::FCFSScheduler() : Scheduler() {}

void FCFSScheduler::run() {
    // Handle empty process list
    if (processes.empty()) {
        std::cerr << "Error: No processes to schedule!" << std::endl;
        return;
    }

    // Sort processes by arrival time
    sortByArrivalTime();

    int currentTime = 0;

    // Execute each process in FCFS order
    for (auto& process : processes) {
        // Handle CPU idle time
        if (currentTime < process.arrivalTime) {
            currentTime = process.arrivalTime;
        }

        // Process starts execution
        int startTime = currentTime;
        int completionTime = startTime + process.burstTime;

        // Update process metrics
        process.completionTime = completionTime;
        process.responseTime = startTime - process.arrivalTime;
        process.waitingTime = startTime - process.arrivalTime;
        process.turnaroundTime = completionTime - process.arrivalTime;
        process.remainingTime = 0;  // Process completed

        // Move timeline forward
        currentTime = completionTime;
    }

    // Store total execution time
    totalExecutionTime = currentTime;
}

void FCFSScheduler::calculateMetrics() {
    if (processes.empty()) {
        return;
    }

    // Use base class helper to compute average metrics
    computeAverageMetrics();
}

void FCFSScheduler::displayResults() const {
    Utils::printHeader(getAlgorithmName());
    printProcessTable();
    std::cout << std::endl;
    drawGanttChart();
    std::cout << std::endl;
    printMetricsSummary();
}

std::string FCFSScheduler::getAlgorithmName() const {
    return "FCFS - First Come First Served Scheduling";
}

void FCFSScheduler::drawGanttChart() const {
    if (processes.empty()) {
        return;
    }

    Utils::printHeader("Gantt Chart");

    // Print process blocks
    std::cout << " ";
    for (const auto& p : processes) {
        std::cout << std::setw(8) << std::right << ("| P" + std::to_string(p.pid));
    }
    std::cout << " |" << std::endl;

    // Print timeline (completion times)
    std::cout << std::setw(2) << "0";
    for (const auto& p : processes) {
        std::cout << std::setw(8) << std::right << p.completionTime;
    }
    std::cout << std::endl;

    // Print bottom border
    std::cout << " ";
    for (size_t i = 0; i < processes.size(); ++i) {
        std::cout << std::string(9, '-');
    }
    std::cout << std::endl;
}

void FCFSScheduler::printProcessTable() const {
    if (processes.empty()) {
        std::cout << "No processes to display." << std::endl;
        return;
    }

    Utils::printHeader("Process Execution Details");

    // Print table header
    std::cout << std::left 
              << std::setw(6) << "PID"
              << std::setw(10) << "Arrival"
              << std::setw(10) << "Burst"
              << std::setw(12) << "Completion"
              << std::setw(12) << "Waiting"
              << std::setw(12) << "Turnaround"
              << std::setw(12) << "Response"
              << std::endl;
    Utils::printDivider(74, '-');

    // Print each process
    for (const auto& p : processes) {
        std::cout << std::left
                  << std::setw(6) << p.pid
                  << std::setw(10) << p.arrivalTime
                  << std::setw(10) << p.burstTime
                  << std::setw(12) << p.completionTime
                  << std::setw(12) << p.waitingTime
                  << std::setw(12) << p.turnaroundTime
                  << std::setw(12) << p.responseTime
                  << std::endl;
    }
}

void FCFSScheduler::printMetricsSummary() const {
    Utils::printHeader("Scheduling Metrics");

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Waiting Time:      " << metrics.averageWaitingTime << " time units" << std::endl;
    std::cout << "Average Turnaround Time:   " << metrics.averageTurnaroundTime << " time units" << std::endl;
    std::cout << "Average Response Time:     " << metrics.averageResponseTime << " time units" << std::endl;
    std::cout << "Total Execution Time:      " << totalExecutionTime << " time units" << std::endl;
    std::cout << "CPU Utilization:           " << metrics.cpuUtilization << "%" << std::endl;
    std::cout << "Throughput:                " << std::fixed << std::setprecision(4) 
              << metrics.throughput << " processes/time unit" << std::endl;
}

int FCFSScheduler::calculateResponseTime(const Process& process) const {
    // In FCFS, response time equals waiting time
    // (process gets CPU access immediately when it's its turn)
    return process.waitingTime;
}
