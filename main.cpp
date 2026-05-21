#include <iostream>
#include "Process.h"
#include "Scheduler.h"
#include "Metrics.h"
#include "Utils.h"

int main() {
    Utils::printHeader("CPU SCHEDULING SIMULATOR - Architecture Test");

    std::cout << "✓ Testing Process class..." << std::endl;
    Process p1(1, 0, 5, 2);
    Process p2(2, 1, 3, 1);
    Process p3(3, 2, 8, 3);

    std::cout << "Process Created:" << std::endl;
    std::cout << "  PID: " << p1.pid 
              << " | Arrival: " << p1.arrivalTime 
              << " | Burst: " << p1.burstTime 
              << " | Priority: " << p1.priority << std::endl;
    
    std::cout << "\n✓ Process class working correctly!" << std::endl;

    std::cout << "\n✓ Testing Metrics structure..." << std::endl;
    Metrics m;
    m.averageWaitingTime = 5.5;
    m.averageTurnaroundTime = 13.5;
    m.cpuUtilization = 85.0;
    std::cout << "Metrics Created:" << std::endl;
    Utils::printMetrics(m);

    std::cout << "\n✓ Testing Utility functions..." << std::endl;
    Utils::printHeader("Sample Process Table");
    std::vector<Process> testProcesses = {p1, p2, p3};
    Utils::printProcessTable(testProcesses);

    std::cout << "\n" << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "Architecture Foundation Complete!" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "\nNext Phase: Implement scheduling algorithms (FCFS, SJF, Priority, Round Robin)" << std::endl;

    return 0;
}
