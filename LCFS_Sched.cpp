#include "LCFS_Sched.h"

// Returns the next process according to this type of scheduler
Process* LCFS_Sched::get_next_process() {
    if (this->ready_q.empty()) {
        return nullptr;
    }
    Process *proc = ready_q.back();
    ready_q.pop_back();
    return proc;
}
