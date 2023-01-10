#ifndef PROCESS_H
#define PROCESS_H
#include "Number_Generator.h"

class Process {
private:
    static int p_counter;
public:
    int state;
    int pid;
    int arrival_time;
    int total_cpu_time;
    int c_burst_limit;
    int i_burst_limit;
    int static_prio;
    int time_remaining;
    int dynamic_prio;
    int io_in = 0;
    int io_out = 0;
    int cpu_wait_time = 0;
    int io_wait_time = 0;
    int finish_time = 0;
    int c_burst = 0;
    int i_burst = 0;
    int state_ts = 1;
    int prev_state = 0;
    virtual ~Process();
    void update_state(int new_state);
    void update_remaining_time(int timestamp);
    static int calc_new_burst(int burst_limit);
    void update_c_burst();
    void update_i_burst();
    int get_io_time() const;
    void update_cpu_wait_time(int wait_time);
    void update_io_wait_time(int wait_time);
    int get_turnaround_time() const;
    Process(int current_state,
            int arrival_time,
            int total_cpu_time,
            int c_burst_limit,
            int i_burst_limit,
            int max_prios) {
        this->state = current_state;
        this->pid = p_counter++;
        this->arrival_time = arrival_time;
        this->total_cpu_time = total_cpu_time;
        this->time_remaining = total_cpu_time;
        this->c_burst_limit = c_burst_limit;
        this->i_burst_limit = i_burst_limit;
        this->static_prio = Number_Generator::my_random(max_prios);
        this->dynamic_prio = this->static_prio-1;
    }
};

#endif //PROCESS_H
