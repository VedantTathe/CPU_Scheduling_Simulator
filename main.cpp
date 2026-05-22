#include <iostream>
#include "Process.h"
#include "Scheduler.h"
#include "Metrics.h"
#include "Utils.h"
#include "Comparison.h"
#include "algorithms/FCFS.h"
#include "algorithms/SJF.h"
#include "algorithms/PriorityScheduler.h"

int main() {
    Utils::printHeader("CPU SCHEDULING SIMULATOR - FCFS vs SJF vs Priority Comparison");

    // Create sample processes showing differences between all three algorithms
    // Phase 5 Test Data: Demonstrates priority-driven scheduling
    std::vector<Process> processes = {
        Process(1, 0, 7, 3),   // P1: arrival=0, burst=7, priority=3 (low)
        Process(2, 2, 4, 1),   // P2: arrival=2, burst=4, priority=1 (HIGH)
        Process(3, 4, 1, 2),   // P3: arrival=4, burst=1, priority=2 (MEDIUM)
        Process(4, 5, 4, 4)    // P4: arrival=5, burst=4, priority=4 (lowest)
    };

    std::cout << "Sample Processes (Lower priority # = Higher priority):" << std::endl;
    for (const auto& p : processes) {
        std::cout << "  P" << p.pid << ": arrival=" << p.arrivalTime 
                  << " burst=" << p.burstTime << " priority=" << p.priority << std::endl;
    }
    std::cout << "\nNote: Priority scheduler will execute P2 before P3 (priority 1 vs 2)" << std::endl;

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

    // Run Priority Scheduler
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "RUNNING PRIORITY SCHEDULER" << std::endl;
    std::cout << std::string(80, '=') << std::endl << std::endl;

    PriorityScheduler priority;
    for (const auto& p : processes) {
        priority.addProcess(p);
    }
    priority.run();
    priority.calculateMetrics();
    priority.displayResults();

    // Algorithm Comparison
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "ALGORITHM COMPARISON - ALL THREE SCHEDULERS" << std::endl;
    std::cout << std::string(80, '=') << std::endl << std::endl;

    AlgorithmComparison comparison;
    comparison.addScheduler(&fcfs, processes);
    comparison.addScheduler(&sjf, processes);
    comparison.addScheduler(&priority, processes);
    comparison.runAllSchedulers();
    comparison.displayComparison();

    std::cout << "\n" << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "Simulation Complete - Phase 5: FCFS vs SJF vs Priority Scheduling" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;

    return 0;
}
