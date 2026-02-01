#ifndef BEING_SERVER_H
#define BEING_SERVER_H

#include "../../core/alchemist/properties.h"

const int max_delay_move = 100; // 1 sec.
const int max_delay_grow = 100; // 600; // 600=1 min.

class BeingServer
{
  public:
    Property * age;
    Property * max_age;
    bool alive;
    int padding;
    BeingServer();
    void show(bool details = true);
    Property ** get_properties(int * count);
    ~BeingServer();
    virtual bool grow();
    virtual bool tick();
};

#endif // BEING_SERVER_H