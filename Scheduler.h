#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "Process.h"
#include <queue>
#include <list>
#include <algorithm>

using namespace std;

class Scheduler {
public:
    string name;
    deque<Process*> ready_q;
    list<Process*> done_pro_list;
    virtual ~Scheduler();
    virtual Process* get_next_process() = 0;
    virtual void add_to_ready_q(Process* process);
    virtual void update_pro_dyn_prio(Process* process);
    void print_ready_q();
    virtual bool test_preempt();
};

#endif
