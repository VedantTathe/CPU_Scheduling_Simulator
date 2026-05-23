#include "FCFS.h"
#include "../include/Utils.h"
#include "../include/ContextSwitch.h"
#include "../include/SchedulerRegistry.h"
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

    // Record initial ready transitions at each process's arrival time
    for (auto& process : processes) {
        recordTransition(process.arrivalTime, process.pid, ProcessState::WAITING, ProcessState::READY);
        process.state = ProcessState::READY;
    }

    int currentTime = 0;
    Process* previousProcess = nullptr;

    // Execute each process in FCFS order
    for (auto& process : processes) {
        // Handle CPU idle time
        if (currentTime < process.arrivalTime) {
            currentTime = process.arrivalTime;
            previousProcess = nullptr; // CPU was idle
        }

        // Perform context switch if enabled
        if (contextSwitchEnabled) {
            ContextSwitch::performSwitch(currentTime, previousProcess, &process, contextSwitchRealTimeDelayMs);
            currentTime += contextSwitchDelay;
        }

        // Process starts execution
        int startTime = currentTime;
        int completionTime = startTime + process.burstTime;

        // Transition from READY to RUNNING
        recordTransition(startTime, process.pid, ProcessState::READY, ProcessState::RUNNING);
        process.state = ProcessState::RUNNING;

        // Update process metrics
        process.completionTime = completionTime;
        process.responseTime = startTime - process.arrivalTime;
        process.waitingTime = startTime - process.arrivalTime;
        process.turnaroundTime = completionTime - process.arrivalTime;
        process.remainingTime = 0;  // Process completed

        // Transition from RUNNING to COMPLETED
        recordTransition(completionTime, process.pid, ProcessState::RUNNING, ProcessState::COMPLETED);
        process.state = ProcessState::COMPLETED;

        // Move timeline forward
        currentTime = completionTime;
        previousProcess = &process;
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
    printTransitionTrace();
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
    Utils::printEnhancedGanttChart(processes, totalExecutionTime);
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

namespace {
    struct FCFSRegister {
        FCFSRegister() {
            SchedulerRegistry::getInstance().registerScheduler(
                "FCFS",
                "First Come First Served (Non-Preemptive)",
                []() { return std::make_unique<FCFSScheduler>(); }
            );
        }
    };
    static FCFSRegister global_fcfs_register;
}
