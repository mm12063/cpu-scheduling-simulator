#include "FCFS_Sched.h"

// Returns the next process according to this type of scheduler
Process* FCFS_Sched::get_next_process() {
    if (this->ready_q.empty()) {
        return nullptr;
    }
    Process *proc = ready_q.front();
    ready_q.pop_front();
    return proc;
}
