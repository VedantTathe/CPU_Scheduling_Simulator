#ifndef SJF_H
#define SJF_H

#include "Scheduler.h"

class SJF : public Scheduler {
public:
    SJF(vector<Process> p, int cs_time = 0);
    void run() override;
};

#endif
