#ifndef FCFS_H
#define FCFS_H

#include "Scheduler.h"

class FCFS : public Scheduler {
public:
    FCFS(vector<Process> p);
    void run() override;
};

#endif
