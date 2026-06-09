#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>
#include "Process.h"

using namespace std;

// Demonstrating ABSTRACTION & INHERITANCE base class
class Scheduler {
protected:
    vector<Process> processes;
    vector<string> gantt_order;
    vector<int> gantt_time;

    int context_switch_time;

public:
    Scheduler(vector<Process> p, int cs_time = 0);
    virtual ~Scheduler() = default;
    
    // Pure virtual function - must be implemented by derived classes
    virtual void run() = 0;

    void printMetrics();
    void printGanttChart();
};

#endif
