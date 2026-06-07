#ifndef PRIORITYSCHEDULER_H
#define PRIORITYSCHEDULER_H

#include "Scheduler.h"

class PriorityScheduler : public Scheduler {
public:
    PriorityScheduler(vector<Process> p);
    void run() override;
};

#endif
