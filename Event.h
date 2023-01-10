#ifndef EVENT_H
#define EVENT_H
#include "Process.h"

class Event {
public:
    Process *event_process;
    int transition;
    int event_timestamp;
    virtual ~Event();
    Event(int event_timestamp, Process* process, int transition) {
        this->event_timestamp = event_timestamp;
        this->event_process = process;
        this->transition = transition;
    }
};

#endif //EVENT_H
