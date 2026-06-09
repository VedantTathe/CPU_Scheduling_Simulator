#include "SRTF.h"
#include <iostream>

using namespace std;

SRTF::SRTF(vector<Process> p, int cs_time) : Scheduler(p, cs_time) {}

void SRTF::run() {
    cout << "\n>>> Shortest Remaining Time First (SRTF - Preemptive) <<<\n";
    int n = processes.size();
    vector<bool> is_completed(n, false);
    int current_time = 0;
    int completed = 0;
    string prev_id = "";
    
    gantt_time.push_back(0);
    
    while (completed != n) {
        int idx = -1;
        int min_remaining = 1e9;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].getArrivalTime() <= current_time && !is_completed[i]) {
                if (processes[i].getRemainingTime() < min_remaining) {
                    min_remaining = processes[i].getRemainingTime();
                    idx = i;
                } else if (processes[i].getRemainingTime() == min_remaining && processes[i].getArrivalTime() < processes[idx].getArrivalTime()) {
                    idx = i;
                }
            }
        }
        
        if (idx != -1) {
            if (prev_id != "" && prev_id != "IDLE" && prev_id != processes[idx].getId() && context_switch_time > 0) {
                current_time += context_switch_time;
                gantt_order.push_back("CS");
                if (gantt_time.size() <= gantt_order.size()) {
                    gantt_time.push_back(current_time);
                } else {
                    gantt_time.back() = current_time;
                }
            }
            
            if (gantt_order.empty() || gantt_order.back() != processes[idx].getId()) {
                gantt_order.push_back(processes[idx].getId());
                if (gantt_time.back() != current_time) {
                    gantt_time.push_back(current_time);
                }
            }
            
            processes[idx].setRemainingTime(processes[idx].getRemainingTime() - 1);
            current_time++;
            
            if (processes[idx].getRemainingTime() == 0) {
                processes[idx].setCompletionTime(current_time);
                processes[idx].setTurnaroundTime(processes[idx].getCompletionTime() - processes[idx].getArrivalTime());
                processes[idx].setWaitingTime(processes[idx].getTurnaroundTime() - processes[idx].getBurstTime());
                is_completed[idx] = true;
                completed++;
            }
            
            if (gantt_time.size() <= gantt_order.size()) {
                gantt_time.push_back(current_time);
            } else {
                gantt_time.back() = current_time;
            }
            
            prev_id = processes[idx].getId();
            
        } else {
            if (gantt_order.empty() || gantt_order.back() != "IDLE") {
                gantt_order.push_back("IDLE");
                if (gantt_time.back() != current_time) {
                    gantt_time.push_back(current_time);
                }
            }
            current_time++;
            
            if (gantt_time.size() <= gantt_order.size()) {
                gantt_time.push_back(current_time);
            } else {
                gantt_time.back() = current_time;
            }
            
            prev_id = "IDLE";
        }
    }
}
