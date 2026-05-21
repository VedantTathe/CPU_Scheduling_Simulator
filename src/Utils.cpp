#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

namespace Utils {

void printDivider(int width, char c) {
    for (int i = 0; i < width; ++i) {
        std::cout << c;
    }
    std::cout << std::endl;
}

void printHeader(const std::string& title) {
    std::cout << std::endl;
    printDivider(80, '=');
    std::cout << std::setw(40) << std::right << title << std::setw(40) << "" << std::endl;
    printDivider(80, '=');
    std::cout << std::endl;
}

void printProcessTable(const std::vector<Process>& processes) {
    if (processes.empty()) {
        std::cout << "No processes to display." << std::endl;
        return;
    }

    std::cout << std::left << std::setw(8) << "PID"
              << std::setw(12) << "Arrival"
              << std::setw(12) << "Burst"
              << std::setw(12) << "Priority"
              << std::setw(12) << "Completion"
              << std::setw(12) << "Waiting"
              << std::setw(12) << "Turnaround" << std::endl;
    printDivider(80, '-');

    for (const auto& p : processes) {
        std::cout << std::left << std::setw(8) << p.pid
                  << std::setw(12) << p.arrivalTime
                  << std::setw(12) << p.burstTime
                  << std::setw(12) << p.priority
                  << std::setw(12) << p.completionTime
                  << std::setw(12) << p.waitingTime
                  << std::setw(12) << p.turnaroundTime << std::endl;
    }
}

void printMetrics(const Metrics& metrics) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Waiting Time:      " << metrics.averageWaitingTime << " ms" << std::endl;
    std::cout << "Average Turnaround Time:   " << metrics.averageTurnaroundTime << " ms" << std::endl;
    std::cout << "Average Response Time:     " << metrics.averageResponseTime << " ms" << std::endl;
    std::cout << "CPU Utilization:           " << metrics.cpuUtilization << "%" << std::endl;
    std::cout << "Throughput:                " << metrics.throughput << " processes/time unit" << std::endl;
}

void sortByArrivalTime(std::vector<Process>& processes) {
    std::sort(processes.begin(), processes.end(),
              [](const Process& a, const Process& b) {
                  return a.arrivalTime < b.arrivalTime;
              });
}

void sortByBurstTime(std::vector<Process>& processes) {
    std::sort(processes.begin(), processes.end(),
              [](const Process& a, const Process& b) {
                  return a.burstTime < b.burstTime;
              });
}

void sortByPriority(std::vector<Process>& processes) {
    std::sort(processes.begin(), processes.end(),
              [](const Process& a, const Process& b) {
                  return a.priority < b.priority;
              });
}

std::string formatTime(int time) {
    std::ostringstream oss;
    oss << time << "ms";
    return oss.str();
}

void printGanttChart(const std::vector<Process>& processes, int totalTime) {
    if (processes.empty()) {
        std::cout << "No processes to display in Gantt chart." << std::endl;
        return;
    }

    std::cout << "Gantt Chart:" << std::endl;
    std::cout << " ";
    for (const auto& p : processes) {
        std::cout << std::setw(6) << std::right << "| P" << p.pid;
    }
    std::cout << " |" << std::endl;

    std::cout << "0";
    int currentTime = 0;
    for (const auto& p : processes) {
        currentTime = p.completionTime;
        std::cout << std::setw(5) << currentTime;
    }
    std::cout << std::endl;
}

} // namespace Utils
