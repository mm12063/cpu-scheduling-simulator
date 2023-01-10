#include "Process.h"

Process::~Process() = default;

int Process::p_counter = 0;

// Updates the process state to the new state and stores
// its current state in the previous state var 
void Process::update_state(int new_state) {
    this->prev_state = this->state;
    this->state = new_state;
}

// Reduces the remaining time for the process
void Process::update_remaining_time(int timestamp) {
    int current_tr = this->time_remaining;
    this->time_remaining = current_tr - timestamp;
}

// Cals the new burst
int Process::calc_new_burst(int burst_limit) {
    return Number_Generator::my_random(burst_limit);
}

// Updates the c burst with new value
void Process::update_c_burst() {
    this->c_burst = Process::calc_new_burst(this->c_burst_limit);
}

// Updates the i burst with new value
void Process::update_i_burst() {
    this->i_burst = Process::calc_new_burst(this->i_burst_limit);
}

// Calculates the turnaround time
int Process::get_turnaround_time() const {
    return this->finish_time - this->arrival_time;
}

int Process::get_io_time() const {
    return this->io_wait_time;
}

// Calculates the wait time
void Process::update_cpu_wait_time(int wait_time) {
    this->cpu_wait_time += wait_time;
}

void Process::update_io_wait_time(int wait_time) {
    this->io_wait_time += wait_time;
}
