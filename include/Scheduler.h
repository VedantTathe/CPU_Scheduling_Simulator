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

public:
    Scheduler(vector<Process> p);
    virtual ~Scheduler() = default;
    
    // Pure virtual function - must be implemented by derived classes
    virtual void run() = 0;

    void printMetrics();
    void printGanttChart();
};

#endif
