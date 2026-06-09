#ifndef PRIORITYSCHEDULER_H
#define PRIORITYSCHEDULER_H

#include "Scheduler.h"

class PriorityScheduler : public Scheduler {
public:
    PriorityScheduler(vector<Process> p, int cs_time = 0);
    void run() override;
};

#endif
