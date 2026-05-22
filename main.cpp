#include <iostream>
#include "Process.h"
#include "Scheduler.h"
#include "Metrics.h"
#include "Utils.h"
#include "Comparison.h"
#include "algorithms/FCFS.h"
#include "algorithms/SJF.h"

int main() {
    Utils::printHeader("CPU SCHEDULING SIMULATOR - FCFS vs SJF Comparison");

    // Create sample processes that show difference between FCFS and SJF
    // Phase 4 Test Data: Demonstrates how SJF changes execution order
    std::vector<Process> processes = {
        Process(1, 0, 7, 0),   // P1: arrives at 0, burst 7
        Process(2, 2, 4, 0),   // P2: arrives at 2, burst 4
        Process(3, 4, 1, 0),   // P3: arrives at 4, burst 1 (shortest)
        Process(4, 5, 4, 0)    // P4: arrives at 5, burst 4
    };

    std::cout << "Sample Processes:" << std::endl;
    for (const auto& p : processes) {
        std::cout << "  P" << p.pid << ": arrival=" << p.arrivalTime 
                  << " burst=" << p.burstTime << std::endl;
    }
    std::cout << "\nNote: SJF will execute P3 before P2 and P4 (shortest job first)" << std::endl;

    // Run FCFS Scheduler
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "RUNNING FCFS SCHEDULER" << std::endl;
    std::cout << std::string(80, '=') << std::endl << std::endl;

    FCFSScheduler fcfs;
    for (const auto& p : processes) {
        fcfs.addProcess(p);
    }
    fcfs.run();
    fcfs.calculateMetrics();
    fcfs.displayResults();

    // Run SJF Scheduler
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "RUNNING SJF SCHEDULER" << std::endl;
    std::cout << std::string(80, '=') << std::endl << std::endl;

    SJFScheduler sjf;
    for (const auto& p : processes) {
        sjf.addProcess(p);
    }
    sjf.run();
    sjf.calculateMetrics();
    sjf.displayResults();

    // Algorithm Comparison
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "ALGORITHM COMPARISON" << std::endl;
    std::cout << std::string(80, '=') << std::endl << std::endl;

    AlgorithmComparison comparison;
    comparison.addScheduler(&fcfs, processes);
    comparison.addScheduler(&sjf, processes);
    comparison.runAllSchedulers();
    comparison.displayComparison();

    std::cout << "\n" << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "Simulation Complete - Phase 4: FCFS vs SJF Scheduling" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;

    return 0;
}
