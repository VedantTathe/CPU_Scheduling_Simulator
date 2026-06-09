#include "RoundRobin.h"
#include <iostream>
#include <algorithm>
#include <queue>

using namespace std;

RoundRobin::RoundRobin(vector<Process> p, int q, int cs_time) : Scheduler(p, cs_time), quantum(q) {}

void RoundRobin::run() {
    cout << "\n>>> Round Robin (RR) Scheduling [Quantum = " << quantum << "] <<<\n";
    int n = processes.size();
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.getArrivalTime() < b.getArrivalTime();
    });
    
    queue<int> q;
    int current_time = 0;
    int completed = 0;
    vector<bool> in_queue(n, false);
    gantt_time.push_back(0);
    string prev_id = "";
    
    if (processes[0].getArrivalTime() == 0) {
        q.push(0);
        in_queue[0] = true;
    }
    
    while (completed != n) {
        if (q.empty()) {
            current_time++;
            if (gantt_order.empty() || gantt_order.back() != "IDLE") {
                gantt_order.push_back("IDLE");
                gantt_time.push_back(current_time);
            } else {
                gantt_time.back() = current_time;
            }
            prev_id = "IDLE";
            
            for (int i = 0; i < n; i++) {
                if (processes[i].getArrivalTime() <= current_time && !in_queue[i] && processes[i].getRemainingTime() > 0) {
                    q.push(i);
                    in_queue[i] = true;
                }
            }
            continue;
        }
        
        int idx = q.front();
        q.pop();
        
        if (prev_id != "" && prev_id != "IDLE" && prev_id != processes[idx].getId() && context_switch_time > 0) {
            current_time += context_switch_time;
            gantt_order.push_back("CS");
            gantt_time.push_back(current_time);
            
            // Because current_time advanced, we must check if any new processes arrived during the context switch!
            for (int i = 0; i < n; i++) {
                if (processes[i].getArrivalTime() <= current_time && !in_queue[i] && processes[i].getRemainingTime() > 0) {
                    q.push(i);
                    in_queue[i] = true;
                }
            }
        }
        
        int execute_time = min(quantum, processes[idx].getRemainingTime());
        gantt_order.push_back(processes[idx].getId());
        current_time += execute_time;
        gantt_time.push_back(current_time);
        prev_id = processes[idx].getId();
        
        processes[idx].setRemainingTime(processes[idx].getRemainingTime() - execute_time);
        
        for (int i = 0; i < n; i++) {
            if (processes[i].getArrivalTime() <= current_time && i != idx && !in_queue[i] && processes[i].getRemainingTime() > 0) {
                q.push(i);
                in_queue[i] = true;
            }
        }
        
        if (processes[idx].getRemainingTime() > 0) {
            q.push(idx);
        } else {
            completed++;
            processes[idx].setCompletionTime(current_time);
            processes[idx].setTurnaroundTime(processes[idx].getCompletionTime() - processes[idx].getArrivalTime());
            processes[idx].setWaitingTime(processes[idx].getTurnaroundTime() - processes[idx].getBurstTime());
        }
    }
}
