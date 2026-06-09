#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H

#include "Scheduler.h"

class RoundRobin : public Scheduler {
private:
    int quantum;
public:
    RoundRobin(vector<Process> p, int q, int cs_time = 0);
    void run() override;
};

#endif
