#ifndef LCFS_SCHED_H
#define LCFS_SCHED_H
#include "Scheduler.h"

class LCFS_Sched : public Scheduler {
public:
    LCFS_Sched(){
        this->name = "LCFS";
    }
    Process* get_next_process() override;
};

#endif //LCFS_SCHED_H
