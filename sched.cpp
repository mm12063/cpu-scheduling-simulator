#include <iostream>
#include <list>
#include <iterator>
#include <unistd.h>
#include <string>
#include "Scheduler.h"
#include "DES_Layer.h"
#include "FCFS_Sched.h"
#include "LCFS_Sched.h"
#include "SRTF_Sched.h"
#include "RR_Sched.h"
#include "PRIO_Sched.h"
#include "PREPRIO_Sched.h"
#include "Number_Generator.h"

using namespace std;
const typedef enum {STATE_CREATED, STATE_READY, STATE_RUNNING, STATE_BLOCKED, STATE_PREEMPT} process_state_t;
const typedef enum {TRANS_TO_READY, TRANS_TO_RUN, TRANS_TO_BLOCK, TRANS_TO_PREEMPT, TRANS_TO_COMPLETE} process_current_state;
const char SCHED_CODES[6] = {'F','L', 'S', 'R', 'P', 'E'};

DES_Layer des_layer;
const int DEFAULT_MAX_PRIOS = 4;
int max_prios = DEFAULT_MAX_PRIOS;
const int DEFAULT_QUANTUM = 10000;
int quantum = DEFAULT_QUANTUM;
int verbose_flag = 0;
int timestamp_flag = 0;
int event_flag = 0;
int preempt_e_flag = 0;

// Returns the English readable states to print
string get_pretty_state(int current_state){
    switch(current_state) {
        case STATE_CREATED:
            return "CREATED";
        case STATE_READY:
            return "READY";
        case STATE_RUNNING:
            return "RUNNG";
        case STATE_BLOCKED:
            return "BLOCK";
        case STATE_PREEMPT:
            return "PREEMPT";
        default:
            return "";
    }
}

// Prints the trace of the process for debugging
void print_trace(Process* process, int current_time){
    cout << current_time << " ";
    cout << process->pid << " ";
    if (process->prev_state == STATE_CREATED)
        cout << 0 << ": ";
    else
        cout << current_time - process->state_ts << ": ";

    cout << get_pretty_state(process->prev_state) << " -> ";
    cout << get_pretty_state(process->state) << "";

    if (process->prev_state != STATE_CREATED | process->state == STATE_READY) {
        if (process->state == STATE_RUNNING | process->prev_state == STATE_RUNNING)
            cout << " cb=" << process->c_burst;
        if (process->state == STATE_BLOCKED)
            cout << "  ib=" << process->i_burst;
        if (process->state != STATE_READY | process->prev_state == STATE_RUNNING)
            cout << " rem=" << process->time_remaining;
        if (process->state == STATE_RUNNING | process->prev_state == STATE_RUNNING)
            cout << " prio=" << process->dynamic_prio;
    }
    cout << endl;
}

// Prints the trace of the process stats
void print_proc_stats(list<Process*>* done_pro_list, int tot_sim_time, const string& scheduler_name, vector<pair<int,int>> io_tracker){
    cout << scheduler_name << endl;
    int total_run_time_total = 0;
    int avg_turn_total = 0;
    int avg_wait_total = 0;
    int counter = 0;
    int num_processes = 0;

    for(Process* pro: *done_pro_list) {
        total_run_time_total += pro->total_cpu_time;
        avg_turn_total += pro->get_turnaround_time();
        avg_wait_total += pro->cpu_wait_time;
        num_processes += 1;
    }

    while (counter <= num_processes) {
        for(Process* pro: *done_pro_list) {
            if (pro->pid == counter)
                printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
                       pro->pid,
                       pro->arrival_time,
                       pro->total_cpu_time,
                       pro->c_burst_limit,
                       pro->i_burst_limit,
                       pro->static_prio,
                       pro->finish_time,
                       pro->get_turnaround_time(),
                       pro->get_io_time(),
                       pro->cpu_wait_time);
        }
        counter++;
    }

    sort(io_tracker.begin(), io_tracker.end());
    int bottom = io_tracker.front().first;
    int top = io_tracker.front().second;
    int total_io = 0;
    int main_total = 0;
    for(pair<int,int> in_out_pair: io_tracker) {
        if (in_out_pair.first < top and in_out_pair.second > top) {
            top = in_out_pair.second;
        }
        if (in_out_pair.first >= top) {
            main_total += top - bottom;
            bottom = in_out_pair.first;
            top = in_out_pair.second;
        }
    }
    total_io = (top - bottom) + main_total;

    double cpu_utilization = 100.0 * (total_run_time_total / (double)tot_sim_time);
    double avg_io = 100.0 * (total_io / (double) tot_sim_time);
    double avg_turnaround = avg_turn_total / (double) done_pro_list->size();
    double avg_wait = avg_wait_total /(double) done_pro_list->size();
    double throughput = 100.0 * ((double) done_pro_list->size() / (double) tot_sim_time);
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
           tot_sim_time,
           cpu_utilization,
           avg_io,
           avg_turnaround,
           avg_wait,
           throughput);
}

// Prints the ready q
string create_printable_ready_q() {
    string str_to_return;
    for(Event* event: des_layer.get_event_queue()){
        str_to_return += "  " + to_string(event->event_timestamp);
        str_to_return += ":";
        str_to_return += to_string(event->event_process->pid);
        str_to_return += ":";
        str_to_return += get_pretty_state(event->transition+1);
    }
    return str_to_return;
}

// Prints the event trace
void print_event_trace(Event* event){
    cout << "  AddEvent(" << event->event_timestamp << ":" << event->event_process->pid << ":";
    cout << get_pretty_state(event->transition + 1) << "):" << create_printable_ready_q();
    cout << "==>   " << to_string(event->event_timestamp) << ":" << to_string(event->event_process->pid);
    cout << ":" << get_pretty_state(event->transition+1) << " ";
    cout << create_printable_ready_q() <<endl;
}


// Main section of code which runs the simulation
void simulation(Scheduler *scheduler) {
    Event *next_event;
    bool call_scheduler = false;
    int current_time = 0;
    int total_sim_time = 0;
    vector<pair<int,int>> io_tracker;
    Process *current_running_process = nullptr;

    while (des_layer.has_next_event()) {
        next_event = des_layer.get_event();
        Process *process = next_event->event_process;
        current_time = next_event->event_timestamp;
        int time_in_prev_state = current_time - process->state_ts;
        int transition = next_event->transition;
        delete next_event; next_event = nullptr;

        switch (transition) {
            case TRANS_TO_READY: {

                if (process->state == STATE_BLOCKED) {
                    process->update_io_wait_time(time_in_prev_state);
                    process->io_out = current_time;
                    io_tracker.emplace_back(process->io_in,process->io_out);
                    process->dynamic_prio = process->static_prio - 1;
                }

                process->update_state(STATE_READY);
                scheduler->add_to_ready_q(process);

                if (verbose_flag)
                    print_trace(process, current_time);

                if (scheduler->test_preempt() and current_running_process != nullptr) {
                    Event *next_event_for_curr_pro = des_layer.get_next_event_for_current_process(current_running_process);
                    if (preempt_e_flag) {
                        cout << "---> PRIO preemption "<<current_running_process->pid<<" by ";
                        cout << process->pid<<" ? "<<(process->dynamic_prio >= current_running_process->dynamic_prio);
                        cout << " TS="<<next_event_for_curr_pro->event_timestamp<<" now="<<current_time<<") --> ";
                    }

                    if (process->dynamic_prio > current_running_process->dynamic_prio and
                        next_event_for_curr_pro->event_timestamp > current_time) {
                        if (preempt_e_flag) {
                            cout << "YES" << endl;
                            cout << "RemoveEvent(" << current_running_process->pid << ")";
                            cout << create_printable_ready_q() << " ==>  ";
                        }
                        des_layer.remove_from_event_q(next_event_for_curr_pro);
                        if (preempt_e_flag)
                            cout << create_printable_ready_q() << endl;

                        // Preempt it NOW
                        auto *event = new Event(current_time, current_running_process, TRANS_TO_PREEMPT);
                        if (event_flag)
                            print_event_trace(event);
                        des_layer.add_to_event_q(event);
                    } else {
                        if (preempt_e_flag)
                            cout << "NO" << endl;
                    }
                }

                call_scheduler = true;
                break;
            }
            
            case TRANS_TO_RUN: {

                if (process->c_burst <= 0)
                    process->update_c_burst();

                if (process->state == STATE_READY)
                    process->update_cpu_wait_time(time_in_prev_state);

                process->update_state(STATE_RUNNING);

                int next_transition, timestamp_next, value_to_check;

                if (process->time_remaining < process->c_burst)
                    process->c_burst = process->time_remaining;

                if (process->c_burst <= process->time_remaining){
                    timestamp_next = process->c_burst + current_time;
                    value_to_check = process->c_burst;
                    next_transition = TRANS_TO_BLOCK;
                }

                if (process->c_burst > quantum) {
                    timestamp_next = quantum + current_time;
                    value_to_check = quantum;
                    next_transition = TRANS_TO_PREEMPT;
                }

                if (verbose_flag)
                    print_trace(process, current_time);

                int check = process->time_remaining - value_to_check;
                if (check > 0) {
                    auto *event = new Event(timestamp_next, process, next_transition);
                    if (event_flag)
                        print_event_trace(event);
                    des_layer.add_to_event_q(event);
                } else {
                    // Process is done.
                    process->finish_time = current_time + value_to_check;
                    auto *event = new Event(process->finish_time, process, TRANS_TO_COMPLETE);
                    des_layer.add_to_event_q(event);
                }
                break;
            }

            case TRANS_TO_BLOCK: {
                process->update_state(STATE_BLOCKED);
                process->update_remaining_time(time_in_prev_state);
                process->io_in = current_time;
                process->c_burst = process->c_burst - time_in_prev_state;

                process->update_i_burst();
                int timestamp = current_time + process->i_burst;
                auto *ready_event = new Event(timestamp, process, TRANS_TO_READY);

                if (verbose_flag)
                    print_trace(process, current_time);
                if (event_flag)
                    print_event_trace(ready_event);
                if (timestamp_flag)
                    scheduler->print_ready_q();

                des_layer.add_to_event_q(ready_event);
                call_scheduler = true;
                current_running_process = nullptr;

                break;
            }

            case TRANS_TO_PREEMPT: {
                process->c_burst -= time_in_prev_state;
                process->update_remaining_time(time_in_prev_state);
                process->update_state(STATE_READY);
                scheduler->update_pro_dyn_prio(process);
                scheduler->add_to_ready_q(process);
                call_scheduler = true;
                current_running_process = nullptr;
                if (verbose_flag)
                    print_trace(process, current_time);
                break;
            }

            case TRANS_TO_COMPLETE: {
                scheduler->done_pro_list.push_back(process);
                if (verbose_flag) {
                    cout << current_time << " ";
                    cout << process->pid << " ";
                    cout << process->c_burst << ": Done" << endl;
                }
                current_running_process = nullptr;
                call_scheduler = true;
                break;
            }
        }

        process->state_ts = current_time;
        if (current_running_process != nullptr)
            total_sim_time = current_time + process->time_remaining;

        if (call_scheduler) {
            if (des_layer.get_next_event_time() == current_time)
                continue;

            call_scheduler = false;
            if (current_running_process == nullptr) {
                current_running_process = scheduler->get_next_process();
                if (current_running_process == nullptr)
                    continue;

                auto *event = new Event(current_time, current_running_process, TRANS_TO_RUN);
                if (event_flag)
                    print_event_trace(event);

                des_layer.add_to_event_q(event);
            }
        }
    }

    print_proc_stats(&scheduler->done_pro_list,
                     total_sim_time,
                     scheduler->name,
                     io_tracker);
}

// Init the processes to be used in the simulation
void init(const vector<vector<int>>& process_spec_data) {

    for(const vector<int>& spec_data:process_spec_data) {
        int timestamp = spec_data.at(0),
            total_cpu_time = spec_data.at(1),
            c_burst = spec_data.at(2),
            i_burst = spec_data.at(3);

        auto *pro = new Process(STATE_CREATED,
                                   timestamp,
                                   total_cpu_time,
                                   c_burst,
                                   i_burst,
                                   max_prios);

        auto *event = new Event(timestamp,
                                 pro,
                                 TRANS_TO_READY);

        des_layer.add_to_event_q(event);
    }
}

vector<string> get_s_data(string svalue) {
    vector<string> s_data;
    if (!svalue.empty()) {
        s_data.push_back(svalue.substr(0,1));
    }
    svalue.erase(0,1);

    if (!svalue.empty()){
        if (svalue.find(':') != -1) {
            int colon_idx = (int)svalue.find(':');
            s_data.push_back(svalue.substr(0,colon_idx));
            if (!svalue.substr(colon_idx+1,svalue.size()).empty())
                s_data.push_back(svalue.substr(colon_idx+1,svalue.size()));
        } else {
            s_data.push_back(svalue);
        }
    }
    return s_data;
}

// Checks if the given code is a valid scheduler code
bool is_valid_sched_code(const string& sched_code){
    return find(begin(SCHED_CODES), end(SCHED_CODES),
                     *sched_code.c_str()) != end(SCHED_CODES);
}

// Returns the scheduler requested in the input params
Scheduler* get_scheduler(const string& sched_code){
    switch (*sched_code.c_str()) {
        case('F'):
            return new FCFS_Sched;
        case('L'):
            return new LCFS_Sched;
        case('S'):
            return new SRTF_Sched;
        case('R'):
            return new RR_Sched(quantum);
        case('P'):
            return new PRIO_Sched(quantum,max_prios);
        case('E'):
            return new PREPRIO_Sched(quantum, max_prios);
        default:
            return nullptr;
    }
}

int main (int argc, char **argv)
{
    Scheduler *scheduler;
    string svalue;
    vector<string> s_data;
    int index, c;
    string input_file;
    string rfile;
    string sched_code;
    vector<string> file_locals;

    while ((c = getopt(argc, argv, "vteps:")) != -1)
        switch (c)
        {
            case 'v':
                verbose_flag = 1;
                break;
            case 't':
                timestamp_flag = 1;
                break;
            case 'e':
                event_flag = 1;
                break;
            case 'p':
                preempt_e_flag = 1;
                break;
            case 's':
                svalue = optarg;
                s_data = get_s_data(svalue);
                break;
            case '?':
                if (optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                return 1;
            default:
                abort();
        }

    for (index = optind; index < argc; index++)
        file_locals.emplace_back(argv[index]);

    if (file_locals.size() == 2 and !s_data.empty()) {
        input_file = file_locals.at(0);
        rfile = file_locals.at(1);

        Number_Generator::populate_rand_numbers(rfile);
        vector<vector<int>> process_spec_data = DES_Layer::create_pro_lst_frm_file(input_file);

        if (s_data.size() > 1)
            quantum = stoi(s_data.at(1));

        if (s_data.size() > 2)
            max_prios = stoi(s_data.at(2));

        if (!process_spec_data.empty())
            init(process_spec_data);
        else
            exit(0);

        if (is_valid_sched_code(s_data.at(0))){
            scheduler = get_scheduler(s_data.at(0));
            if (scheduler != nullptr)
                simulation(scheduler);
            else
                cout << "Unable to locate scheduler for given code." << endl;
        } else {
            cout << "Invalid scheduler code!" << endl;
        }
    } else {
        cout << "Missing file locations – can't proceed! Please check the syntax of your input." << endl;
    }

    return 0;
}
