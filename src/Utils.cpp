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
    int paddingLeft = (80 - title.length()) / 2;
    int paddingRight = 80 - title.length() - paddingLeft;
    std::cout << std::string(paddingLeft, ' ') << title << std::string(paddingRight, ' ') << std::endl;
    printDivider(80, '=');
    std::cout << std::endl;
}

void printSectionHeader(const std::string& title) {
    std::cout << std::endl;
    std::cout << ">> " << title << std::endl;
    printDivider(78, '-');
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
    std::cout << std::endl;
    std::cout << "--- Performance Metrics ---" << std::endl;
    std::cout << "  Average Waiting Time:       " << metrics.averageWaitingTime << " time units" << std::endl;
    std::cout << "  Average Turnaround Time:    " << metrics.averageTurnaroundTime << " time units" << std::endl;
    std::cout << "  Average Response Time:      " << metrics.averageResponseTime << " time units" << std::endl;
    std::cout << "  CPU Utilization:            " << metrics.cpuUtilization << "%" << std::endl;
    std::cout << "  Throughput:                 " << metrics.throughput << " processes/time unit" << std::endl;
    std::cout << std::endl;
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

    printEnhancedGanttChart(processes, totalTime);
}

void printEnhancedGanttChart(const std::vector<Process>& processes, int totalTime) {
    (void)totalTime;  // Use totalTime parameter
    if (processes.empty()) {
        std::cout << "No processes to display in Gantt chart." << std::endl;
        return;
    }

    printSectionHeader("Gantt Chart Visualization");

    // Build idle segments by detecting gaps in execution timeline
    std::vector<std::pair<int, int>> idleSegments;  // (startTime, duration)
    
    if (!processes.empty()) {
        // Check for idle at start (if first process doesn't arrive at time 0)
        if (processes[0].arrivalTime > 0) {
            idleSegments.push_back({0, processes[0].arrivalTime});
        }
        
        // Check for gaps between consecutive processes in timeline
        for (size_t i = 1; i < processes.size(); ++i) {
            int prevCompletionTime = processes[i-1].completionTime;
            int nextArrivalTime = processes[i].arrivalTime;
            int nextStartTime = std::max(prevCompletionTime, nextArrivalTime);
            
            // Idle occurs if there's a gap between end of previous and start of next
            if (nextStartTime > prevCompletionTime) {
                idleSegments.push_back({prevCompletionTime, nextStartTime - prevCompletionTime});
            }
        }
    }

    // Print top border
    std::cout << "+";
    for (const auto& process : processes) {
        std::cout << std::string(6, '-') << "+";
    }
    std::cout << std::endl;

    // Print process boxes
    std::cout << "|";
    for (const auto& process : processes) {
        std::cout << " P" << std::setfill(' ') << std::setw(3) << process.pid << " |";
    }
    std::cout << std::endl;

    // Print middle border
    std::cout << "+";
    for (size_t i = 0; i < processes.size(); ++i) {
        std::cout << std::string(6, '-') << "+";
    }
    std::cout << std::endl;

    // Print time values
    std::cout << "|";
    for (const auto& process : processes) {
        std::cout << std::setfill(' ') << std::setw(5) << process.completionTime << " |";
    }
    std::cout << std::endl;

    // Print bottom border
    std::cout << "+";
    for (size_t i = 0; i < processes.size(); ++i) {
        std::cout << std::string(6, '-') << "+";
    }
    std::cout << std::endl;

    // Print time axis
    std::cout << "0";
    for (const auto& process : processes) {
        std::cout << std::string(6, ' ') << process.completionTime;
    }
    std::cout << std::endl;
    std::cout << std::endl;

    // Show idle periods if any
    if (!idleSegments.empty()) {
        printInfo("CPU was idle during:");
        for (const auto& idle : idleSegments) {
            std::cout << "  -> Time " << idle.first << " to " << (idle.first + idle.second) 
                      << " (duration: " << idle.second << " units)" << std::endl;
        }
        std::cout << std::endl;
    }

    // Get last completion time
    int lastCompletionTime = 0;
    if (!processes.empty()) {
        lastCompletionTime = processes.back().completionTime;
    }
    std::cout << "Total Execution Time: " << lastCompletionTime << " time units" << std::endl;
}

void printSuccess(const std::string& message) {
    std::cout << "[OK] " << message << std::endl;
}

void printWarning(const std::string& message) {
    std::cout << "[!] " << message << std::endl;
}

void printInfo(const std::string& message) {
    std::cout << "[i] " << message << std::endl;
}

} // namespace Utils
