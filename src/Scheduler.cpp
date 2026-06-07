#include "Scheduler.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

using namespace std;

Scheduler::Scheduler(vector<Process> p) : processes(p) {}

void Scheduler::printMetrics() {
    int n = processes.size();
    float total_wt = 0, total_tat = 0;

    cout << "\n------------------------------------------------------------\n";
    cout << "ID\tArrival\tBurst\tPriority\tCompletion\tTAT\tWT\n";
    cout << "------------------------------------------------------------\n";

    for (int i = 0; i < n; i++) {
        total_wt += processes[i].getWaitingTime();
        total_tat += processes[i].getTurnaroundTime();
        
        cout << processes[i].getId() << "\t"
             << processes[i].getArrivalTime() << "\t"
             << processes[i].getBurstTime() << "\t"
             << processes[i].getPriority() << "\t\t"
             << processes[i].getCompletionTime() << "\t\t"
             << processes[i].getTurnaroundTime() << "\t"
             << processes[i].getWaitingTime() << "\n";
    }
    
    cout << "------------------------------------------------------------\n";
    cout << "Average Turnaround Time: " << (total_tat / n) << endl;
    cout << "Average Waiting Time: " << (total_wt / n) << endl;
    
    int max_completion = 0;
    int total_burst = 0;
    for(auto p : processes) {
        max_completion = max(max_completion, p.getCompletionTime());
        total_burst += p.getBurstTime();
    }
    cout << "CPU Utilization: " << fixed << setprecision(2) << ((float)total_burst / max_completion) * 100 << "%\n\n";
}

void Scheduler::printGanttChart() {
    if (gantt_order.empty()) return;

    cout << "\n--- Gantt Chart ---\n";
    for (size_t i = 0; i < gantt_order.size(); i++) cout << "--------";
    cout << "\n|";
    
    for (size_t i = 0; i < gantt_order.size(); i++) {
        cout << "  " << gantt_order[i] << "\t|";
    }
    cout << "\n";
    
    for (size_t i = 0; i < gantt_order.size(); i++) cout << "--------";
    cout << "\n";
    
    cout << gantt_time[0];
    for (size_t i = 1; i < gantt_time.size(); i++) {
        cout << "\t" << gantt_time[i];
    }
    cout << "\n\n";
}
