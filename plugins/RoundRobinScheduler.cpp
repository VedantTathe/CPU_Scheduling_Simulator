#include "../include/Scheduler.h"
#include "../include/SchedulerRegistry.h"
#include "../include/Utils.h"
#include "../include/ContextSwitch.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <string>

/**
 * @class RoundRobinScheduler
 * @brief Preemptive Round Robin CPU scheduling algorithm implementation.
 */
class RoundRobinScheduler : public Scheduler {
private:
    mutable int quantum = 0;

public:
    RoundRobinScheduler() : Scheduler() {}
    ~RoundRobinScheduler() override = default;

    int getQuantum() const override {
        if (quantum <= 0) {
            std::cout << "Enter Time Quantum (default: 2): ";
            std::string qInput;
            std::getline(std::cin, qInput);
            if (qInput.empty()) {
                quantum = 2;
            } else {
                try {
                    quantum = std::stoi(qInput);
                    if (quantum <= 0) quantum = 2;
                } catch (...) {
                    quantum = 2;
                }
            }
            std::cout << "[OK] Using Time Quantum: " << quantum << " units\n" << std::endl;
        }
        return quantum;
    }

    void run() override {
        if (processes.empty()) {
            std::cerr << "Error: No processes to schedule!" << std::endl;
            return;
        }

        // Sort initially by arrival time to scan arrivals properly
        sortByArrivalTime();

        // Set all processes to WAITING state
        for (auto& p : processes) {
            p.state = ProcessState::WAITING;
            p.remainingTime = p.burstTime;
            p.responseTime = -1;
        }

        int currentTime = 0;
        std::vector<Process*> readyQueue;
        Process* previousProcess = nullptr;

        // Helper to push all newly arrived processes onto readyQueue
        auto checkNewArrivals = [&](int fromTime, int toTime) {
            // Find processes arriving in [fromTime, toTime]
            for (auto& p : processes) {
                if (p.state == ProcessState::WAITING && p.arrivalTime >= fromTime && p.arrivalTime <= toTime) {
                    p.state = ProcessState::READY;
                    recordTransition(p.arrivalTime, p.pid, ProcessState::WAITING, ProcessState::READY);
                    readyQueue.push_back(&p);
                }
            }
        };

        // Initial arrivals check at time 0
        checkNewArrivals(0, 0);

        int completedCount = 0;
        int n = processes.size();

        while (completedCount < n) {
            // If readyQueue is empty, jump to next arrival time
            if (readyQueue.empty()) {
                int nextArrival = -1;
                for (const auto& p : processes) {
                    if (p.state == ProcessState::WAITING) {
                        if (nextArrival == -1 || p.arrivalTime < nextArrival) {
                            nextArrival = p.arrivalTime;
                        }
                    }
                }
                if (nextArrival != -1) {
                    currentTime = nextArrival;
                    checkNewArrivals(currentTime, currentTime);
                } else {
                    break;
                }
            }

            // Pop process from queue
            Process* p = readyQueue.front();
            readyQueue.erase(readyQueue.begin());

            // Perform context switch if enabled
            if (contextSwitchEnabled && previousProcess != p) {
                ContextSwitch::performSwitch(currentTime, previousProcess, p, contextSwitchRealTimeDelayMs);
                currentTime += contextSwitchDelay;
                // Record context switch time additions for arrivals
                checkNewArrivals(currentTime - contextSwitchDelay + 1, currentTime);
            }

            // Determine run duration
            int currentQuantum = getQuantum();
            int runTime = std::min(currentQuantum, p->remainingTime);

            // Record response time if first execution
            if (p->responseTime < 0) {
                p->responseTime = currentTime - p->arrivalTime;
                if (p->responseTime < 0) p->responseTime = 0;
            }

            // Transition: READY -> RUNNING
            recordTransition(currentTime, p->pid, ProcessState::READY, ProcessState::RUNNING);
            p->state = ProcessState::RUNNING;

            int startTime = currentTime;
            currentTime += runTime;
            p->remainingTime -= runTime;

            // Check for arrivals during execution
            checkNewArrivals(startTime + 1, currentTime);

            // Transition after execution
            if (p->remainingTime <= 0) {
                // Completed!
                recordTransition(currentTime, p->pid, ProcessState::RUNNING, ProcessState::COMPLETED);
                p->state = ProcessState::COMPLETED;
                p->completionTime = currentTime;
                p->turnaroundTime = p->completionTime - p->arrivalTime;
                p->waitingTime = p->turnaroundTime - p->burstTime;
                if (p->waitingTime < 0) p->waitingTime = 0;

                completedCount++;
                previousProcess = p;
            } else {
                // Quantum expired, preempt!
                recordTransition(currentTime, p->pid, ProcessState::RUNNING, ProcessState::READY);
                p->state = ProcessState::READY;
                // Push back to the end of readyQueue
                readyQueue.push_back(p);
                previousProcess = p;
            }
        }

        totalExecutionTime = currentTime;
    }

    void calculateMetrics() override {
        if (processes.empty()) {
            return;
        }
        computeAverageMetrics();
    }

    void displayResults() const override {
        Utils::printHeader(getAlgorithmName());
        printTransitionTrace();
        printProcessTable();
        std::cout << std::endl;
        drawGanttChart();
        std::cout << std::endl;
        printMetricsSummary();
    }

    std::string getAlgorithmName() const override {
        return "Round Robin (Preemptive)";
    }

private:
    void drawGanttChart() const {
        if (processes.empty()) return;
        Utils::printHeader("Gantt Chart");
        Utils::printEnhancedGanttChart(processes, totalExecutionTime);
    }

    void printProcessTable() const {
        if (processes.empty()) {
            std::cout << "No processes to display." << std::endl;
            return;
        }

        Utils::printHeader("Process Execution Details");

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

    void printMetricsSummary() const {
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
};

// Plugin export interface
extern "C" {
#ifdef _WIN32
    __declspec(dllexport) void register_plugin(SchedulerRegistry& registry) {
#else
    void register_plugin(SchedulerRegistry& registry) {
#endif
        registry.registerScheduler(
            "Round Robin",
            "Round Robin Scheduling (Preemptive)",
            []() { return std::make_unique<RoundRobinScheduler>(); }
        );
    }
}
