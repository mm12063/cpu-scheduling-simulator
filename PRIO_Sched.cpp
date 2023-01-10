#include "PRIO_Sched.h"
#include <array>

// Returns the next process according to this type of scheduler
Process *PRIO_Sched::get_next_process() {
    Process* next_process = this->get_next_process_from_p_queues();
    if (next_process == nullptr)
        this->swap_queues();
    else
        return next_process;
    return this->get_next_process_from_p_queues();
}

// Returns the highest prio next process from the active/expired Qs
Process *PRIO_Sched::get_next_process_from_p_queues() const {
    for (int i = this->max_prios-1; i >= 0; --i) {
        for (Process* process : this->activeQ[i]) {
            Process* selected_pro = process;
            this->activeQ[i].erase(
                    remove(this->activeQ[i].begin(),
                           this->activeQ[i].end(),
                           selected_pro),
                           this->activeQ[i].end());
            return selected_pro;
        }
    }
    return nullptr;
}

// Swaps the queues once the active q is empty
void PRIO_Sched::swap_queues() {
    deque<Process*> *temp_p = this->activeQ;
    this->activeQ = this->expiredQ;
    this->expiredQ = temp_p;
}

// Adds to the ready queue - either the active or expired q depending on the 
// process' dynamic prio
void PRIO_Sched::add_to_ready_q(Process *process) {
    if (process->dynamic_prio >= 0){
        this->activeQ[process->dynamic_prio].push_back(process);
    } else {
        process->dynamic_prio = process->static_prio-1;
        this->expiredQ[process->dynamic_prio].push_back(process);
    }
}

// Updates the dynamic prio of the process
void PRIO_Sched::update_pro_dyn_prio(Process* process) {
    process->dynamic_prio--;
}