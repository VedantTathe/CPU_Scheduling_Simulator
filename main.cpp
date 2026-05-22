#include <iostream>
#include "Process.h"
#include "Scheduler.h"
#include "Metrics.h"
#include "Utils.h"
#include "algorithms/FCFS.h"

int main() {
    Utils::printHeader("CPU SCHEDULING SIMULATOR - FCFS Implementation");

    // Create FCFS scheduler
    FCFSScheduler fcfs;

    // Add sample processes
    // Format: pid, arrival_time, burst_time, priority
    fcfs.addProcess(1, 0, 5, 0);    // P1: arrives at 0, needs 5 time units
    fcfs.addProcess(2, 1, 3, 0);    // P2: arrives at 1, needs 3 time units
    fcfs.addProcess(3, 2, 4, 0);    // P3: arrives at 2, needs 4 time units
    fcfs.addProcess(4, 6, 2, 0);    // P4: arrives at 6, needs 2 time units

    std::cout << "Processes Added: " << fcfs.getProcessCount() << std::endl;
    std::cout << "Running FCFS Scheduler...\n" << std::endl;

    // Execute the scheduling algorithm
    fcfs.run();

    // Calculate metrics
    fcfs.calculateMetrics();

    // Display results
    fcfs.displayResults();

    std::cout << "\n" << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "FCFS Scheduling Simulation Complete!" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;

    return 0;
}
