#ifndef PREPRIO_SCHED_H
#define PREPRIO_SCHED_H
#include "Scheduler.h"
#include "PRIO_Sched.h"

class PREPRIO_Sched : public PRIO_Sched {
public:
    PREPRIO_Sched(int quantum, int max_prios) : PRIO_Sched(quantum, max_prios) {
        this->name = "PREPRIO " + to_string(quantum);
    }
    bool test_preempt() override;
};

#endif //PREPRIO_SCHED_H