#include "Scheduler.h"
#include "Utils.h"
#include <algorithm>
#include <numeric>

Scheduler::Scheduler()
    : totalExecutionTime(0),
      contextSwitchEnabled(false),
      contextSwitchDelay(0),
      contextSwitchRealTimeDelayMs(0),
      liveVisualization(false) {}

void Scheduler::setContextSwitchSettings(bool enabled, int delay, int realTimeDelayMs, bool live) {
    contextSwitchEnabled = enabled;
    contextSwitchDelay = delay;
    contextSwitchRealTimeDelayMs = realTimeDelayMs;
    liveVisualization = live;
}

void Scheduler::addProcess(const Process& process) {
    processes.push_back(process);
}

void Scheduler::addProcess(int pid, int arrival, int burst, int priority) {
    processes.emplace_back(pid, arrival, burst, priority);
}

void Scheduler::clearProcesses() {
    processes.clear();
    metrics.reset();
    totalExecutionTime = 0;
    transitionEvents.clear();
}

int Scheduler::getProcessCount() const {
    return processes.size();
}

const Metrics& Scheduler::getMetrics() const {
    return metrics;
}

void Scheduler::reset() {
    for (auto& p : processes) {
        p.reset();
    }
    metrics.reset();
    totalExecutionTime = 0;
    transitionEvents.clear();
}

Process* Scheduler::findProcessByPid(int pid) {
    auto it = std::find_if(processes.begin(), processes.end(),
                          [pid](const Process& p) { return p.pid == pid; });
    return (it != processes.end()) ? &(*it) : nullptr;
}

const Process* Scheduler::findProcessByPid(int pid) const {
    auto it = std::find_if(processes.begin(), processes.end(),
                          [pid](const Process& p) { return p.pid == pid; });
    return (it != processes.end()) ? &(*it) : nullptr;
}

std::vector<Process> Scheduler::getCompletedProcesses() const {
    std::vector<Process> completed;
    for (const auto& p : processes) {
        if (p.completionTime > 0) {
            completed.push_back(p);
        }
    }
    return completed;
}

void Scheduler::sortByArrivalTime() {
    std::sort(processes.begin(), processes.end(),
              [](const Process& a, const Process& b) {
                  return a.arrivalTime < b.arrivalTime;
              });
}

void Scheduler::computeAverageMetrics() {
    if (processes.empty()) {
        return;
    }

    double totalWaitingTime = 0.0;
    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;
    int validResponseTime = 0;

    for (const auto& p : processes) {
        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
        if (p.responseTime >= 0) {
            totalResponseTime += p.responseTime;
            validResponseTime++;
        }
    }

    metrics.averageWaitingTime = totalWaitingTime / processes.size();
    metrics.averageTurnaroundTime = totalTurnaroundTime / processes.size();
    metrics.averageResponseTime = (validResponseTime > 0) 
        ? (totalResponseTime / validResponseTime) 
        : 0.0;

    // Calculate CPU utilization
    double totalBurstTime = 0.0;
    for (const auto& p : processes) {
        totalBurstTime += p.burstTime;
    }
    metrics.cpuUtilization = (totalExecutionTime > 0)
        ? ((totalBurstTime / totalExecutionTime) * 100.0)
        : 0.0;

    // Calculate throughput (processes per time unit)
    metrics.throughput = (totalExecutionTime > 0)
        ? (static_cast<double>(processes.size()) / totalExecutionTime)
        : 0.0;
}

void Scheduler::recordTransition(int time, int pid, ProcessState oldState, ProcessState newState) {
    transitionEvents.push_back({time, pid, oldState, newState});
    if (liveVisualization) {
        Utils::printStateTransition(time, pid, oldState, newState);
    }
}

void Scheduler::printTransitionTrace() const {
    if (transitionEvents.empty()) {
        std::cout << "No state transitions recorded." << std::endl;
        return;
    }

    // Copy and sort events chronologically
    auto events = transitionEvents;
    std::sort(events.begin(), events.end());

    Utils::printSectionHeader(getAlgorithmName() + " - Process State Transitions Trace");
    for (const auto& event : events) {
        Utils::printStateTransition(event.time, event.pid, event.oldState, event.newState);
    }
    std::cout << std::endl;
}
