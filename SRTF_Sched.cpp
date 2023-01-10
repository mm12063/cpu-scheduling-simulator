#include "SRTF_Sched.h"

// Gets new process according to this scheduler
Process *SRTF_Sched::get_next_process() {
    if (this->ready_q.empty()) {
        return nullptr;
    }
    Process* shortest = this->ready_q.front();
    for (Process* pro: this->ready_q)
        if (pro->time_remaining < shortest->time_remaining) {
            shortest = pro;
        }
    this->ready_q.erase(remove(this->ready_q.begin(),this->ready_q.end(), shortest),this->ready_q.end());
    return shortest;
}
