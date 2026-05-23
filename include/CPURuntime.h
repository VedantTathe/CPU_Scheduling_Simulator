#pragma once

#include "CPUProcess.h"
#include "Scheduler.h"
#include "CPUCore.h"
#include "TimerInterrupt.h"
#include <vector>
#include <memory>
#include <string>
#include <mutex>

/**
 * @class CPURuntime
 * @brief Coordinates multi-core CPU execution simulation.
 * 
 * Manages worker threads, generates simulated timer interrupts, queries scheduling
 * policies, performs context switches, and logs concurrent state transitions.
 */
class CPURuntime {
private:
    std::vector<std::unique_ptr<CPUCore>> cores;
    TimerInterrupt timer;
    int totalSimulationTime;
    
    // Track previous process ran on each core to log context switch details
    std::vector<Process*> previousProcesses;

    // Mutex to serialize standard console outputs
    mutable std::mutex printMutex;

public:
    CPURuntime();
    ~CPURuntime();

    /**
     * @brief Run the live multi-core thread-based scheduling simulation.
     * 
     * @param simProcesses Vector of processes to schedule and execute
     * @param scheduler Scheduler instance supplying the algorithm name and metrics
     * @param numCores Number of cores to run (1-8)
     * @param switchDelay Context switch simulation delay units
     * @param realTimeDelayMs Real-time step sleep duration in ms
     */
    void run(std::vector<Process>& simProcesses, Scheduler* scheduler, int numCores, int switchDelay, int realTimeDelayMs);

private:
    /**
     * @brief Query scheduling policy to select the next ready process.
     */
    Process* selectNextProcess(const std::vector<Process*>& readyQueue, const std::string& algoName);
};
