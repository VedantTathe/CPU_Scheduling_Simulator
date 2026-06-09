#include "FCFS.h"
#include <iostream>
#include <algorithm>

using namespace std;

FCFS::FCFS(vector<Process> p, int cs_time) : Scheduler(p, cs_time) {}

void FCFS::run() {
    cout << "\n>>> First Come First Serve (FCFS) <<<\n";
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.getArrivalTime() < b.getArrivalTime();
    });
    
    int current_time = 0;
    gantt_time.push_back(0);
    string prev_id = "";

    for (size_t i = 0; i < processes.size(); i++) {
        if (current_time < processes[i].getArrivalTime()) {
            current_time = processes[i].getArrivalTime();
            gantt_order.push_back("IDLE");
            gantt_time.push_back(current_time);
            prev_id = "IDLE";
        }
        
        if (prev_id != "" && prev_id != "IDLE" && prev_id != processes[i].getId() && context_switch_time > 0) {
            current_time += context_switch_time;
            gantt_order.push_back("CS");
            gantt_time.push_back(current_time);
        }
        
        current_time += processes[i].getBurstTime();
        processes[i].setCompletionTime(current_time);
        processes[i].setTurnaroundTime(processes[i].getCompletionTime() - processes[i].getArrivalTime());
        processes[i].setWaitingTime(processes[i].getTurnaroundTime() - processes[i].getBurstTime());
        
        gantt_order.push_back(processes[i].getId());
        gantt_time.push_back(current_time);
        prev_id = processes[i].getId();
    }
}
