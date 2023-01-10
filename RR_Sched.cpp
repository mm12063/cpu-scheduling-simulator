#include "RR_Sched.h"

// Gets new process according to this scheduler
Process *RR_Sched::get_next_process() {
    if (this->ready_q.empty()) {
        return nullptr;
    }
    Process *proc = ready_q.front();
    ready_q.pop_front();
    return proc;
}
