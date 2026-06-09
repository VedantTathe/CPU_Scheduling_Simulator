#ifndef SRTF_H
#define SRTF_H

#include "Scheduler.h"

class SRTF : public Scheduler {
public:
    SRTF(vector<Process> p, int cs_time = 0);
    void run() override;
};

#endif
