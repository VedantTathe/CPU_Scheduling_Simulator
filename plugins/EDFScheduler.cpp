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
 * @class EDFScheduler
 * @brief Preemptive Earliest Deadline First (EDF) CPU scheduling algorithm implementation.
 */
class EDFScheduler : public Scheduler {
public:
    EDFScheduler() : Scheduler() {}
    ~EDFScheduler() override = default;

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
        Process* runningProcess = nullptr;
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
            // Sort readyQueue by deadline (earliest absolute deadline first), then PID
            std::sort(readyQueue.begin(), readyQueue.end(), [](const Process* a, const Process* b) {
                if (a->deadline != b->deadline) return a->deadline < b->deadline;
                return a->pid < b->pid;
            });
        };

        // Initial arrivals check at time 0
        checkNewArrivals(0, 0);

        int completedCount = 0;
        int n = processes.size();

        while (completedCount < n) {
            // Check if a running process can be preempted by an earlier deadline process in readyQueue
            if (runningProcess != nullptr && !readyQueue.empty()) {
                if (readyQueue.front()->deadline < runningProcess->deadline) {
                    // Preempt currently running process!
                    recordTransition(currentTime, runningProcess->pid, ProcessState::RUNNING, ProcessState::READY);
                    runningProcess->state = ProcessState::READY;
                    readyQueue.push_back(runningProcess);
                    // Resort readyQueue
                    std::sort(readyQueue.begin(), readyQueue.end(), [](const Process* a, const Process* b) {
                        if (a->deadline != b->deadline) return a->deadline < b->deadline;
                        return a->pid < b->pid;
                    });
                    previousProcess = runningProcess;
                    runningProcess = nullptr;
                }
            }

            // If no process is running and readyQueue is empty, advance time to next arrival
            if (runningProcess == nullptr && readyQueue.empty()) {
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

            // If no process is running, pick the one with earliest deadline from readyQueue
            if (runningProcess == nullptr && !readyQueue.empty()) {
                runningProcess = readyQueue.front();
                readyQueue.erase(readyQueue.begin());

                // Perform context switch if enabled and previous process is different
                if (contextSwitchEnabled && previousProcess != runningProcess) {
                    ContextSwitch::performSwitch(currentTime, previousProcess, runningProcess, contextSwitchRealTimeDelayMs);
                    currentTime += contextSwitchDelay;
                    // Check arrivals during context switch delay
                    checkNewArrivals(currentTime - contextSwitchDelay + 1, currentTime);
                }

                // Record response time if first execution
                if (runningProcess->responseTime < 0) {
                    runningProcess->responseTime = currentTime - runningProcess->arrivalTime;
                    if (runningProcess->responseTime < 0) runningProcess->responseTime = 0;
                }

                // Transition: READY -> RUNNING
                recordTransition(currentTime, runningProcess->pid, ProcessState::READY, ProcessState::RUNNING);
                runningProcess->state = ProcessState::RUNNING;
            }

            // Run for 1 time unit (simulation tick) to allow preemptions to be checked dynamically at each arrival tick
            int startTime = currentTime;
            currentTime += 1;
            runningProcess->remainingTime -= 1;

            // Check for arrivals during this tick
            checkNewArrivals(startTime + 1, currentTime);

            // If process completed
            if (runningProcess->remainingTime <= 0) {
                recordTransition(currentTime, runningProcess->pid, ProcessState::RUNNING, ProcessState::COMPLETED);
                runningProcess->state = ProcessState::COMPLETED;
                runningProcess->completionTime = currentTime;
                runningProcess->turnaroundTime = runningProcess->completionTime - runningProcess->arrivalTime;
                runningProcess->waitingTime = runningProcess->turnaroundTime - runningProcess->burstTime;
                if (runningProcess->waitingTime < 0) runningProcess->waitingTime = 0;

                completedCount++;
                previousProcess = runningProcess;
                runningProcess = nullptr;
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
        return "Earliest Deadline First (EDF) Preemptive";
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
                  << std::setw(12) << "Deadline"
                  << std::setw(12) << "Completion"
                  << std::setw(12) << "Waiting"
                  << std::setw(12) << "Turnaround"
                  << std::setw(12) << "Response"
                  << std::endl;
        Utils::printDivider(86, '-');

        for (const auto& p : processes) {
            std::cout << std::left
                      << std::setw(6) << p.pid
                      << std::setw(10) << p.arrivalTime
                      << std::setw(10) << p.burstTime
                      << std::setw(12) << p.deadline
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
            "EDF",
            "Earliest Deadline First Scheduling (Preemptive)",
            []() { return std::make_unique<EDFScheduler>(); }
        );
    }
}
