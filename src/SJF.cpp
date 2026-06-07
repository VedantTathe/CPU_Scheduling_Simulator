#include "SJF.h"
#include <iostream>

using namespace std;

SJF::SJF(vector<Process> p) : Scheduler(p) {}

void SJF::run() {
    cout << "\n>>> Shortest Job First (SJF) <<<\n";
    int n = processes.size();
    vector<bool> is_completed(n, false);
    int current_time = 0;
    int completed = 0;
    gantt_time.push_back(0);
    
    while (completed != n) {
        int idx = -1;
        int min_burst = 1e9;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].getArrivalTime() <= current_time && !is_completed[i]) {
                if (processes[i].getBurstTime() < min_burst) {
                    min_burst = processes[i].getBurstTime();
                    idx = i;
                }
                if (processes[i].getBurstTime() == min_burst && processes[i].getArrivalTime() < processes[idx].getArrivalTime()) {
                    idx = i;
                }
            }
        }
        
        if (idx != -1) {
            current_time += processes[idx].getBurstTime();
            processes[idx].setCompletionTime(current_time);
            processes[idx].setTurnaroundTime(processes[idx].getCompletionTime() - processes[idx].getArrivalTime());
            processes[idx].setWaitingTime(processes[idx].getTurnaroundTime() - processes[idx].getBurstTime());
            is_completed[idx] = true;
            completed++;
            
            gantt_order.push_back(processes[idx].getId());
            gantt_time.push_back(current_time);
        } else {
            current_time++;
            if (gantt_order.empty() || gantt_order.back() != "IDLE") {
                gantt_order.push_back("IDLE");
            }
            if (gantt_time.back() != current_time) {
                if (gantt_order.back() == "IDLE" && gantt_time.size() > 1) {
                    gantt_time.back() = current_time;
                } else {
                    gantt_time.push_back(current_time);
                }
            }
        }
    }
}
