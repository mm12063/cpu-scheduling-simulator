#ifndef DES_LAYER_H
#define DES_LAYER_H
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <list>
#include <vector>
#include "Event.h"
#include "Scheduler.h"
#include <sstream>
#include <algorithm>

using namespace std;

class DES_Layer {
protected:
    list<Event*> event_queue;
public:
    virtual ~DES_Layer();
    static vector<vector<int>> create_pro_lst_frm_file(const string& file_location);
    void add_to_event_q(Event* event);
    int get_next_event_time();
    void remove_from_event_q(Event* event);
    list<Event*> get_event_queue();
    Event* get_event();
    bool has_next_event();
    Event* get_next_event_for_current_process(Process * process);
};

#endif //DES_LAYER_H
