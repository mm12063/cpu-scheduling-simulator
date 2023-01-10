#ifndef SRTF_SCHED_H
#define SRTF_SCHED_H
#include "Scheduler.h"

class SRTF_Sched : public Scheduler {
public:
    SRTF_Sched(){
        this->name = "SRTF";
    }
    Process* get_next_process() override;
};

#endif //SRTF_SCHED_H