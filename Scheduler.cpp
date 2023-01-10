#include "Scheduler.h"

Scheduler::~Scheduler() = default;

// Base add to ready q
void Scheduler::add_to_ready_q(Process *process) {
    this->ready_q.push_back(process);
}

// Prints the ready q - To assist with debugging
void Scheduler::print_ready_q() {
    cout << "SCHED (" << this->ready_q.size() << "): ";
    for(Process* pro: this->ready_q){
        cout << " " << pro->pid << ":" << pro->state_ts;
    }
    cout << endl;
}

// Updates the dynamic pro
void Scheduler::update_pro_dyn_prio(Process* process) {
    process->dynamic_prio = process->static_prio-1;
}

// Checks if we need to test for preempt
// false for all except certain (E) schedulers
bool Scheduler::test_preempt() {
    return false;
}
