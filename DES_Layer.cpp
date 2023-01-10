#include "DES_Layer.h"
using namespace std;

DES_Layer::~DES_Layer() = default;

// Returns the event q
list<Event*> DES_Layer::get_event_queue() {
    return this->event_queue;
}

// Removes the passed event to the method from the event q
void DES_Layer::remove_from_event_q(Event* event){
    this->event_queue.erase(remove(this->event_queue.begin(),this->event_queue.end(), event),this->event_queue.end());
}

// Add an event object to the event q
void DES_Layer::add_to_event_q(Event* event){
    list<Event*>::iterator it;
    it = event_queue.begin();
    if (!event_queue.empty()) {
        for (Event* queued_event: event_queue){
            if (queued_event->event_timestamp > event->event_timestamp) {
                break;
            }
            ++it;
        }
    }
    event_queue.insert(it, event);
}

// Receives the location of a file to parse and returns vector of the data
vector<vector<int>> DES_Layer::create_pro_lst_frm_file(const string& file_location) {
    ifstream file;
    string file_line;
    string word;
    vector<vector<int>> process_spec_lst;

    file.open(file_location);
    if (file.is_open()) {
        while (getline(file, file_line)) {
            istringstream line(file_line);
            vector<int> temp_list;
            while (getline(line, word, ' ')) {
                if (!word.empty()){
                    temp_list.push_back(stoi(word));
                }
            }
            process_spec_lst.push_back(temp_list);
        }
    } else {
        cout << "Unable to open input file." << endl;
        exit(0);
    }
    file.close();
    return process_spec_lst;
}

// Returns the next event
Event* DES_Layer::get_event() {
    Event *next_event = event_queue.front();
    event_queue.pop_front();
    return next_event;
}

// Checks the event q has a next event
bool DES_Layer::has_next_event() {
    if (!event_queue.empty()){
        return true;
    }
    return false;
}

// Returns the next event from the event q
int DES_Layer::get_next_event_time() {
    if (event_queue.empty())
       return 0;
    return event_queue.front()->event_timestamp;
}

// Returns the next event for the current running process
Event* DES_Layer::get_next_event_for_current_process(Process *current_process) {
    for (Event *event : this->event_queue)
        if (event->event_process->pid == current_process->pid)
            return event;
    return nullptr;
}
