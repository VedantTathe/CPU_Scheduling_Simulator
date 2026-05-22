#include "PriorityScheduler.h"
#include "../include/Utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

PriorityScheduler::PriorityScheduler() : Scheduler() {}

Process* PriorityScheduler::selectNextProcess(int currentTime, const std::vector<int>& scheduled) {
    Process* highest = nullptr;
    int highestPriority = INT_MAX;

    // Find unscheduled process with highest priority among arrived processes
    // Lower numeric value = higher priority
    for (auto& process : processes) {
        // Check if process has arrived and not yet scheduled
        bool isScheduled = std::find(scheduled.begin(), scheduled.end(), process.pid) != scheduled.end();
        
        if (process.arrivalTime <= currentTime && !isScheduled) {
            // Lower priority number is better
            if (process.priority < highestPriority) {
                highestPriority = process.priority;
                highest = &process;
            }
        }
    }

    return highest;
}

void PriorityScheduler::run() {
    // Handle empty process list
    if (processes.empty()) {
        std::cerr << "Error: No processes to schedule!" << std::endl;
        return;
    }

    std::vector<int> scheduled;  // Track which processes have been scheduled
    int currentTime = 0;

    // Continue until all processes are scheduled
    while (scheduled.size() < processes.size()) {
        // Find next process to execute (highest priority among arrived)
        Process* nextProcess = selectNextProcess(currentTime, scheduled);

        // If no process has arrived, advance to next arrival
        if (nextProcess == nullptr) {
            int nextArrival = INT_MAX;
            for (const auto& p : processes) {
                bool isScheduled = std::find(scheduled.begin(), scheduled.end(), p.pid) != scheduled.end();
                if (!isScheduled && p.arrivalTime > currentTime) {
                    nextArrival = std::min(nextArrival, p.arrivalTime);
                }
            }
            if (nextArrival != INT_MAX) {
                currentTime = nextArrival;
                nextProcess = selectNextProcess(currentTime, scheduled);
            }
        }

        if (nextProcess != nullptr) {
            // Execute the selected process
            int startTime = currentTime;
            int completionTime = startTime + nextProcess->burstTime;

            nextProcess->completionTime = completionTime;
            nextProcess->responseTime = startTime - nextProcess->arrivalTime;
            nextProcess->waitingTime = startTime - nextProcess->arrivalTime;
            nextProcess->turnaroundTime = completionTime - nextProcess->arrivalTime;
            nextProcess->remainingTime = 0;

            scheduled.push_back(nextProcess->pid);
            currentTime = completionTime;
        }
    }

    totalExecutionTime = currentTime;
}

void PriorityScheduler::calculateMetrics() {
    if (processes.empty()) {
        return;
    }

    computeAverageMetrics();
}

void PriorityScheduler::displayResults() const {
    Utils::printHeader(getAlgorithmName());
    printProcessTable();
    std::cout << std::endl;
    drawGanttChart();
    std::cout << std::endl;
    printMetricsSummary();
}

std::string PriorityScheduler::getAlgorithmName() const {
    return "Priority Scheduling (Non-Preemptive)";
}

void PriorityScheduler::drawGanttChart() const {
    if (processes.empty()) {
        return;
    }

    Utils::printHeader("Gantt Chart");

    // Create ordered list by completion time for chart
    std::vector<const Process*> ordered;
    for (const auto& p : processes) {
        ordered.push_back(&p);
    }
    std::sort(ordered.begin(), ordered.end(),
              [](const Process* a, const Process* b) {
                  return a->completionTime < b->completionTime;
              });

    // Convert to non-const vector for enhanced chart display
    std::vector<Process> sortedProcesses;
    for (const auto* p : ordered) {
        sortedProcesses.push_back(*p);
    }

    Utils::printEnhancedGanttChart(sortedProcesses, totalExecutionTime);
}

void PriorityScheduler::printProcessTable() const {
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
              << std::setw(10) << "Priority"
              << std::setw(12) << "Completion"
              << std::setw(12) << "Waiting"
              << std::setw(12) << "Turnaround"
              << std::setw(12) << "Response"
              << std::endl;
    Utils::printDivider(84, '-');

    // Print each process
    for (const auto& p : processes) {
        std::cout << std::left
                  << std::setw(6) << p.pid
                  << std::setw(10) << p.arrivalTime
                  << std::setw(10) << p.burstTime
                  << std::setw(10) << p.priority
                  << std::setw(12) << p.completionTime
                  << std::setw(12) << p.waitingTime
                  << std::setw(12) << p.turnaroundTime
                  << std::setw(12) << p.responseTime
                  << std::endl;
    }
}

void PriorityScheduler::printMetricsSummary() const {
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
