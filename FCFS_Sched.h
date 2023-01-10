#ifndef FCFS_SCHED_H
#define FCFS_SCHED_H
#include "Scheduler.h"

class FCFS_Sched : public Scheduler {
public:
    FCFS_Sched(){
        this->name = "FCFS";
    }
    Process* get_next_process() override;
};

#endif //FCFS_SCHED_H

