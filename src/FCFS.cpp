#include "FCFS.h"
#include <iostream>
#include <algorithm>

using namespace std;

FCFS::FCFS(vector<Process> p) : Scheduler(p) {}

void FCFS::run() {
    cout << "\n>>> First Come First Serve (FCFS) <<<\n";
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.getArrivalTime() < b.getArrivalTime();
    });
    
    int current_time = 0;
    gantt_time.push_back(0);

    for (size_t i = 0; i < processes.size(); i++) {
        if (current_time < processes[i].getArrivalTime()) {
            current_time = processes[i].getArrivalTime();
            gantt_order.push_back("IDLE");
            gantt_time.push_back(current_time);
        }
        
        current_time += processes[i].getBurstTime();
        processes[i].setCompletionTime(current_time);
        processes[i].setTurnaroundTime(processes[i].getCompletionTime() - processes[i].getArrivalTime());
        processes[i].setWaitingTime(processes[i].getTurnaroundTime() - processes[i].getBurstTime());
        
        gantt_order.push_back(processes[i].getId());
        gantt_time.push_back(current_time);
    }
}
