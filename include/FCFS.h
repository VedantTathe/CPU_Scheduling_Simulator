#ifndef FCFS_H
#define FCFS_H

#include "Scheduler.h"

class FCFS : public Scheduler {
public:
    FCFS(vector<Process> p, int cs_time = 0);
    void run() override;
};

#endif
