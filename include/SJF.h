#ifndef SJF_H
#define SJF_H

#include "Scheduler.h"

class SJF : public Scheduler {
public:
    SJF(vector<Process> p);
    void run() override;
};

#endif
