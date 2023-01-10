#ifndef RR_SCHED_H
#define RR_SCHED_H
#include "Scheduler.h"

class RR_Sched : public Scheduler {
public:
    explicit RR_Sched(int quantum){
        this->name = "RR " + to_string(quantum);
    }
    Process* get_next_process() override;
};

#endif //RR_SCHED_H