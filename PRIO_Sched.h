#ifndef PRIO_SCHED_H
#define PRIO_SCHED_H
#include "Scheduler.h"

class PRIO_Sched : public Scheduler {
public:
    deque<Process*> *activeQ, *expiredQ;
    int max_prios = 0;
    PRIO_Sched(int quantum, int max_prios){
        this->max_prios = max_prios;
        this->name = "PRIO " + to_string(quantum);
        this->activeQ = new deque<Process*>[max_prios];
        this->expiredQ = new deque<Process*>[max_prios];
    }
    Process* get_next_process() override;
    Process* get_next_process_from_p_queues() const;
    void swap_queues();
    void add_to_ready_q(Process* process) override;
    void update_pro_dyn_prio(Process* process) override;
};

#endif //PRIO_SCHED_H